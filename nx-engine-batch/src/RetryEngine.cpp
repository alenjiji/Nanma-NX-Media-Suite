#include "nx/batch/RetryEngine.h"

namespace nx::batch {

RetryAttempt RetryAttempt::create_initial(SessionId session_id, std::string job_value) {
    return RetryAttempt{
        .attempt_id = SessionJobId::create_initial(std::move(session_id), std::move(job_value)),
        .parent_attempt_id = std::nullopt,
        .retry_index = 0
    };
}

RetryAttempt RetryAttempt::create_retry(const RetryAttempt& parent_attempt) {
    return RetryAttempt{
        .attempt_id = SessionJobId::create_retry(parent_attempt.attempt_id),
        .parent_attempt_id = parent_attempt.attempt_id,
        .retry_index = parent_attempt.retry_index + 1
    };
}

RetryChain RetryChain::create(JobExecutionSpec intent, RetryAttempt initial_attempt) {
    std::vector<RetryAttempt> attempts;
    attempts.push_back(std::move(initial_attempt));
    
    return RetryChain{
        .intent = std::move(intent),
        .attempts = std::move(attempts)
    };
}

RetryAttempt RetryChain::add_retry(const RetryAttempt& parent_attempt) {
    auto retry_attempt = RetryAttempt::create_retry(parent_attempt);
    attempts.push_back(retry_attempt);
    return retry_attempt;
}

const RetryAttempt& RetryChain::current_attempt() const {
    return attempts.back();
}

size_t RetryChain::attempt_count() const {
    return attempts.size();
}

RetryExecutor::RetryExecutor(ExecutionRecorder* recorder)
    : recorder_(recorder) {
}

JobExecutionResult RetryExecutor::execute_retry(
    const JobExecutionSpec& intent,
    const RetryAttempt& attempt
) {
    // Execute job with immutable intent - this is a stub implementation
    // In a real system, this would delegate to the actual job executor
    JobExecutionResult result{
        .success = true,
        .message = "Retry execution completed",
        .result_token = "retry_token_" + attempt.attempt_id.job_value
    };
    
    // Record execution attempt for replay if recorder is available
    if (recorder_) {
        auto execution_record = ExecutionRecord::create(
            attempt.attempt_id,
            attempt.parent_attempt_id,
            attempt.retry_index,
            intent,
            result_to_outcome(result)
        );
        recorder_->record(execution_record);
    }
    
    return result;
}

ExecutionOutcome RetryExecutor::result_to_outcome(const JobExecutionResult& result) const {
    return result.success ? ExecutionOutcome::success() : ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed);
}

} // namespace nx::batch