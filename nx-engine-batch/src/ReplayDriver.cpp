#include "nx/batch/ReplayDriver.h"
#include <unordered_map>
#include <algorithm>

namespace nx::batch {

ReplayDriver::ReplayDriver(std::shared_ptr<RetryExecutor> retry_executor)
    : retry_executor_(std::move(retry_executor)) {
}

ReplayReport ReplayDriver::replay_and_verify(const ExecutionReplaySource& source) {
    // Load all persisted execution records
    auto records = source.load_all();
    
    if (records.empty()) {
        return ReplayReport::success();
    }
    
    // Reconstruct retry chains from persisted records
    auto retry_chains = reconstruct_retry_chains(records);
    
    // Replay all execution attempts and collect mismatches
    std::vector<ReplayMismatch> mismatches;
    
    for (const auto& record : records) {
        // Generate fresh session ID for replay
        auto replay_session_id = generate_replay_session_id();
        
        // Create fresh retry attempt for replay
        auto replay_attempt = RetryAttempt{
            .attempt_id = SessionJobId::create_initial(replay_session_id, "replay-" + std::to_string(record.retry_index)),
            .parent_attempt_id = std::nullopt,  // Simplified for replay
            .retry_index = record.retry_index
        };
        
        // Execute replay attempt with identical intent
        auto replay_result = retry_executor_->execute_retry(record.intent, replay_attempt);
        
        // Verify attempt against original record
        auto mismatch = verify_attempt(record, replay_result);
        if (mismatch.has_value()) {
            mismatches.push_back(mismatch.value());
        }
    }
    
    // Generate verification report
    if (mismatches.empty()) {
        return ReplayReport::success();
    } else {
        return ReplayReport::divergence(std::move(mismatches));
    }
}

std::vector<RetryChain> ReplayDriver::reconstruct_retry_chains(const std::vector<ExecutionRecord>& records) {
    // Group records by intent hash to reconstruct retry chains
    std::unordered_map<std::string, std::vector<ExecutionRecord>> chains_by_intent;
    
    for (const auto& record : records) {
        chains_by_intent[record.intent.hash.value].push_back(record);
    }
    
    // Reconstruct retry chains
    std::vector<RetryChain> retry_chains;
    
    for (auto& [intent_hash, chain_records] : chains_by_intent) {
        // Sort by retry index to maintain deterministic order
        std::sort(chain_records.begin(), chain_records.end(),
                  [](const ExecutionRecord& a, const ExecutionRecord& b) {
                      return a.retry_index < b.retry_index;
                  });
        
        if (!chain_records.empty()) {
            // Create retry chain from first record
            auto& first_record = chain_records[0];
            auto initial_attempt = RetryAttempt{
                .attempt_id = first_record.attempt_id,
                .parent_attempt_id = first_record.parent_attempt_id,
                .retry_index = first_record.retry_index
            };
            
            auto chain = RetryChain::create(first_record.intent, initial_attempt);
            
            // Add remaining attempts to chain
            for (size_t i = 1; i < chain_records.size(); ++i) {
                auto& record = chain_records[i];
                auto retry_attempt = RetryAttempt{
                    .attempt_id = record.attempt_id,
                    .parent_attempt_id = record.parent_attempt_id,
                    .retry_index = record.retry_index
                };
                chain.attempts.push_back(retry_attempt);
            }
            
            retry_chains.push_back(std::move(chain));
        }
    }
    
    return retry_chains;
}

std::optional<ReplayMismatch> ReplayDriver::verify_attempt(
    const ExecutionRecord& original_record,
    const JobExecutionResult& replay_result
) {
    // Convert replay result to execution outcome for comparison
    ExecutionOutcome replay_outcome = replay_result.success 
        ? ExecutionOutcome::success() 
        : ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed);
    
    // Compare outcomes
    if (original_record.outcome != replay_outcome) {
        return ReplayMismatch{
            .retry_index = original_record.retry_index,
            .expected = original_record.outcome,
            .actual = replay_outcome
        };
    }
    
    return std::nullopt;
}

SessionId ReplayDriver::generate_replay_session_id() const {
    // Generate deterministic replay session ID
    return SessionId{"replay-session"};
}

TestableReplayExecutor::TestableReplayExecutor(ExecutionRecorder* recorder)
    : RetryExecutor(recorder) {
}

void TestableReplayExecutor::force_outcome_for_retry_index(uint32_t retry_index, ExecutionOutcome forced_outcome) {
    forced_outcomes_[retry_index] = forced_outcome;
}

JobExecutionResult TestableReplayExecutor::execute_retry(
    const JobExecutionSpec& intent,
    const RetryAttempt& attempt
) {
    // Check for forced outcome
    auto forced_it = forced_outcomes_.find(attempt.retry_index);
    if (forced_it != forced_outcomes_.end()) {
        return outcome_to_result(forced_it->second, attempt);
    }
    
    // Default to parent implementation
    return RetryExecutor::execute_retry(intent, attempt);
}

JobExecutionResult TestableReplayExecutor::outcome_to_result(const ExecutionOutcome& outcome, const RetryAttempt& attempt) const {
    bool success = (outcome.kind == ExecutionOutcome::Kind::Success);
    std::string message = success ? "Forced success" : "Forced failure";
    std::string token = "forced_token_" + attempt.attempt_id.job_value;
    
    return JobExecutionResult{
        .success = success,
        .message = message,
        .result_token = token
    };
}

} // namespace nx::batch