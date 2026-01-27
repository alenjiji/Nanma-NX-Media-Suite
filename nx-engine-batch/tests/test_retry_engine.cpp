#include "nx/batch/RetryEngine.h"
#include "nx/batch/ExecutionPersistence.h"
#include <cassert>

using namespace nx::batch;

void test_retry_creates_new_session_job_id() {
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Create initial attempt
    auto initial_attempt = RetryAttempt::create_initial(session_id, "job-001");
    
    // Create retry attempt
    auto retry_attempt = RetryAttempt::create_retry(initial_attempt);
    
    // Assert different SessionJobId
    assert(initial_attempt.attempt_id != retry_attempt.attempt_id);
    
    // Assert identical execution inputs (intent)
    RetryExecutor executor;
    auto result1 = executor.execute_retry(intent, initial_attempt);
    auto result2 = executor.execute_retry(intent, retry_attempt);
    
    // Results should be identical in structure (both successful in stub implementation)
    assert(result1.success == result2.success);
    assert(result1.message == result2.message);
    // Tokens will differ due to different attempt IDs, which is expected
}

void test_retry_does_not_mutate_execution_spec() {
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Hash JobExecutionSpec (using its built-in hash)
    auto original_hash = intent.hash;
    
    // Execute original + retry
    auto initial_attempt = RetryAttempt::create_initial(session_id, "job-001");
    auto retry_attempt = RetryAttempt::create_retry(initial_attempt);
    
    RetryExecutor executor;
    executor.execute_retry(intent, initial_attempt);
    executor.execute_retry(intent, retry_attempt);
    
    // Hash again
    auto final_hash = intent.hash;
    
    // Assert bit-identity
    assert(original_hash == final_hash);
}

void test_retry_chain_is_structurally_correct() {
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Create retry chain: original → retry → retry
    auto initial_attempt = RetryAttempt::create_initial(session_id, "job-001");
    auto retry_chain = RetryChain::create(intent, initial_attempt);
    
    auto retry1 = retry_chain.add_retry(initial_attempt);
    auto retry2 = retry_chain.add_retry(retry1);
    
    // Assert parent relationships preserved
    assert(!initial_attempt.parent_attempt_id.has_value());
    assert(retry1.parent_attempt_id.has_value());
    assert(retry1.parent_attempt_id.value() == initial_attempt.attempt_id);
    assert(retry2.parent_attempt_id.has_value());
    assert(retry2.parent_attempt_id.value() == retry1.attempt_id);
    
    // Assert retry indices are stable and ordered
    assert(initial_attempt.retry_index == 0);
    assert(retry1.retry_index == 1);
    assert(retry2.retry_index == 2);
    
    // Assert chain structure
    assert(retry_chain.attempt_count() == 3);
    assert(retry_chain.current_attempt().attempt_id == retry2.attempt_id);
}

void test_replayed_retry_chain_executes_identically() {
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Original execution with persistence
    InMemoryExecutionRecorder recorder;
    RetryExecutor executor(&recorder);
    
    auto initial_attempt = RetryAttempt::create_initial(session_id, "job-001");
    auto retry_attempt = RetryAttempt::create_retry(initial_attempt);
    
    // Execute original chain
    auto result1 = executor.execute_retry(intent, initial_attempt);
    auto result2 = executor.execute_retry(intent, retry_attempt);
    
    // Persist retry chain
    auto original_records = recorder.get_records();
    
    // Reconstruct chain from persistence
    auto replay_source = InMemoryExecutionReplaySource(original_records);
    auto replayed_records = replay_source.load_all();
    
    // Execute all attempts from replayed chain
    RetryExecutor replay_executor;
    std::vector<JobExecutionResult> replay_results;
    
    for (const auto& record : replayed_records) {
        // Create new attempt with fresh SessionJobId for replay
        auto replay_attempt = RetryAttempt{
            .attempt_id = SessionJobId::create_initial(session_id, "replay-" + std::to_string(record.retry_index)),
            .parent_attempt_id = record.parent_attempt_id,
            .retry_index = record.retry_index
        };
        
        auto replay_result = replay_executor.execute_retry(record.intent, replay_attempt);
        replay_results.push_back(replay_result);
    }
    
    // Assert execution order identical
    assert(replayed_records.size() == 2);
    assert(replayed_records[0].retry_index == 0);
    assert(replayed_records[1].retry_index == 1);
    
    // Assert results identical (in structure)
    assert(replay_results.size() == 2);
    assert(replay_results[0].success == result1.success);
    assert(replay_results[1].success == result2.success);
    
    // Assert intent equality across replay
    assert(replayed_records[0].intent == intent);
    assert(replayed_records[1].intent == intent);
    
    // Assert retry index ordering preserved
    assert(replayed_records[0].retry_index < replayed_records[1].retry_index);
}

void test_retry_negative_capabilities() {
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Test: No timestamps present
    auto initial_attempt = RetryAttempt::create_initial(session_id, "job-001");
    auto retry_attempt = RetryAttempt::create_retry(initial_attempt);
    
    // RetryAttempt and RetryChain contain no time fields by design
    // This is enforced by compilation - no time fields exist in the structures
    
    // Test: No resume logic possible
    InMemoryExecutionRecorder recorder;
    RetryExecutor executor(&recorder);
    
    executor.execute_retry(intent, initial_attempt);
    
    // Persistence records only completed executions
    auto records = recorder.get_records();
    assert(records.size() == 1);
    
    // No "in-progress" or "resume" state exists
    // ExecutionRecord contains only completed execution facts
    assert(records[0].outcome.kind == ExecutionOutcome::Kind::Success);
    
    // Test: No shared execution state
    RetryExecutor executor1;
    RetryExecutor executor2;
    
    // Each executor is independent
    auto result1 = executor1.execute_retry(intent, initial_attempt);
    auto result2 = executor2.execute_retry(intent, retry_attempt);
    
    // No shared state between executors
    assert(result1.success == result2.success);  // Both succeed in stub implementation
    
    // Test: No implicit retry triggers
    // RetryExecutor only executes when explicitly called
    // No autonomous behavior exists in the implementation
}

void test_retry_chain_deterministic_ordering() {
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Create multiple retry chains with same parameters
    std::vector<RetryChain> chains;
    
    for (int i = 0; i < 3; ++i) {
        auto initial_attempt = RetryAttempt::create_initial(session_id, "job-001");
        auto chain = RetryChain::create(intent, initial_attempt);
        
        // Add same number of retries
        auto retry1 = chain.add_retry(initial_attempt);
        auto retry2 = chain.add_retry(retry1);
        
        chains.push_back(chain);
    }
    
    // Verify all chains have identical structure
    for (size_t i = 1; i < chains.size(); ++i) {
        assert(chains[0].attempt_count() == chains[i].attempt_count());
        assert(chains[0].intent == chains[i].intent);
        
        // Retry indices should be identical
        for (size_t j = 0; j < chains[0].attempts.size(); ++j) {
            assert(chains[0].attempts[j].retry_index == chains[i].attempts[j].retry_index);
        }
    }
}

int main() {
    test_retry_creates_new_session_job_id();
    test_retry_does_not_mutate_execution_spec();
    test_retry_chain_is_structurally_correct();
    test_replayed_retry_chain_executes_identically();
    test_retry_negative_capabilities();
    test_retry_chain_deterministic_ordering();
    
    return 0;
}