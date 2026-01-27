#include "nx/batch/ReplayDriver.h"
#include "nx/batch/ExecutionPersistence.h"
#include "nx/batch/RetryEngine.h"
#include <cassert>
#include <memory>

using namespace nx::batch;

void test_replay_matches_original_execution() {
    // Create original execution records
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Create execution records for successful execution
    std::vector<ExecutionRecord> records;
    
    auto attempt_id = SessionJobId::create_initial(session_id, "job-001");
    records.push_back(ExecutionRecord::create(
        attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::success()
    ));
    
    // Create replay source and driver
    auto replay_source = InMemoryExecutionReplaySource(records);
    auto retry_executor = std::make_shared<RetryExecutor>();
    ReplayDriver driver(retry_executor);
    
    // Replay and verify
    auto report = driver.replay_and_verify(replay_source);
    
    // Assert deterministic match
    assert(report.deterministic_match == true);
    assert(report.mismatches.empty());
}

void test_replay_detects_outcome_divergence() {
    // Create original execution records with success
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    std::vector<ExecutionRecord> records;
    auto attempt_id = SessionJobId::create_initial(session_id, "job-001");
    records.push_back(ExecutionRecord::create(
        attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::success()  // Original was successful
    ));
    
    // Create testable executor that forces failure
    auto testable_executor = std::make_shared<TestableReplayExecutor>();
    testable_executor->force_outcome_for_retry_index(0, ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed));
    
    auto replay_source = InMemoryExecutionReplaySource(records);
    ReplayDriver driver(testable_executor);
    
    // Replay and verify
    auto report = driver.replay_and_verify(replay_source);
    
    // Assert divergence detected
    assert(report.deterministic_match == false);
    assert(report.mismatches.size() == 1);
    
    // Verify mismatch details
    auto& mismatch = report.mismatches[0];
    assert(mismatch.retry_index == 0);
    assert(mismatch.expected == ExecutionOutcome::success());
    assert(mismatch.actual == ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed));
}

void test_replay_does_not_skip_execution_based_on_past_outcomes() {
    // Create execution records with failed outcome
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    std::vector<ExecutionRecord> records;
    auto attempt_id = SessionJobId::create_initial(session_id, "job-001");
    records.push_back(ExecutionRecord::create(
        attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed)  // Original failed
    ));
    
    // Create executor that tracks execution calls
    class TrackingRetryExecutor : public RetryExecutor {
    public:
        mutable int execution_count = 0;
        
        JobExecutionResult execute_retry(const JobExecutionSpec& intent, const RetryAttempt& attempt) const override {
            execution_count++;
            return RetryExecutor::execute_retry(intent, attempt);
        }
    };
    
    auto tracking_executor = std::make_shared<TrackingRetryExecutor>();
    auto replay_source = InMemoryExecutionReplaySource(records);
    ReplayDriver driver(tracking_executor);
    
    // Replay
    auto report = driver.replay_and_verify(replay_source);
    
    // Assert replay still executed attempt despite past failure
    assert(tracking_executor->execution_count == 1);
    
    // Should match since both fail in the same way
    assert(report.deterministic_match == true);
}

void test_replay_does_not_write_persistence() {
    // Create execution records
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    std::vector<ExecutionRecord> records;
    auto attempt_id = SessionJobId::create_initial(session_id, "job-001");
    records.push_back(ExecutionRecord::create(
        attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::success()
    ));
    
    // Create recorder to track writes
    InMemoryExecutionRecorder recorder;
    auto initial_record_count = recorder.get_records().size();
    
    // Create retry executor with recorder (should not be used during replay)
    auto retry_executor = std::make_shared<RetryExecutor>(&recorder);
    auto replay_source = InMemoryExecutionReplaySource(records);
    ReplayDriver driver(retry_executor);
    
    // Replay
    auto report = driver.replay_and_verify(replay_source);
    
    // Assert no records were written during replay
    assert(recorder.get_records().size() == initial_record_count);
    assert(report.deterministic_match == true);
}

void test_replay_driver_has_no_runtime_dependencies() {
    // This test verifies that ReplayDriver can be constructed and used
    // without any runtime execution engine dependencies
    
    // Create minimal replay setup
    std::vector<ExecutionRecord> empty_records;
    auto replay_source = InMemoryExecutionReplaySource(empty_records);
    
    // Create standalone retry executor (no recorder, no engine dependencies)
    auto retry_executor = std::make_shared<RetryExecutor>();
    
    // Create replay driver - should not require any runtime engines
    ReplayDriver driver(retry_executor);
    
    // Should be able to replay empty records without any dependencies
    auto report = driver.replay_and_verify(replay_source);
    assert(report.deterministic_match == true);
    assert(report.mismatches.empty());
}

void test_replay_preserves_retry_chain_structure() {
    // Create retry chain with multiple attempts
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Create execution records for retry chain
    std::vector<ExecutionRecord> records;
    
    auto initial_attempt_id = SessionJobId::create_initial(session_id, "job-001");
    auto retry1_attempt_id = SessionJobId::create_retry(initial_attempt_id);
    auto retry2_attempt_id = SessionJobId::create_retry(retry1_attempt_id);
    
    // Original attempt failed
    records.push_back(ExecutionRecord::create(
        initial_attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed)
    ));
    
    // First retry failed
    records.push_back(ExecutionRecord::create(
        retry1_attempt_id,
        initial_attempt_id,
        1,
        intent,
        ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed)
    ));
    
    // Second retry succeeded
    records.push_back(ExecutionRecord::create(
        retry2_attempt_id,
        retry1_attempt_id,
        2,
        intent,
        ExecutionOutcome::success()
    ));
    
    // Create testable executor that matches original pattern
    auto testable_executor = std::make_shared<TestableReplayExecutor>();
    testable_executor->force_outcome_for_retry_index(0, ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed));
    testable_executor->force_outcome_for_retry_index(1, ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed));
    // retry_index 2 will use default success behavior
    
    auto replay_source = InMemoryExecutionReplaySource(records);
    ReplayDriver driver(testable_executor);
    
    // Replay and verify
    auto report = driver.replay_and_verify(replay_source);
    
    // Should match the original retry chain pattern
    assert(report.deterministic_match == true);
    assert(report.mismatches.empty());
}

void test_replay_detects_multiple_divergences() {
    // Create multiple execution records
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    std::vector<ExecutionRecord> records;
    
    // Create two attempts, both originally successful
    auto attempt1_id = SessionJobId::create_initial(session_id, "job-001");
    auto attempt2_id = SessionJobId::create_retry(attempt1_id);
    
    records.push_back(ExecutionRecord::create(
        attempt1_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::success()
    ));
    
    records.push_back(ExecutionRecord::create(
        attempt2_id,
        attempt1_id,
        1,
        intent,
        ExecutionOutcome::success()
    ));
    
    // Force both to fail during replay
    auto testable_executor = std::make_shared<TestableReplayExecutor>();
    testable_executor->force_outcome_for_retry_index(0, ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed));
    testable_executor->force_outcome_for_retry_index(1, ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed));
    
    auto replay_source = InMemoryExecutionReplaySource(records);
    ReplayDriver driver(testable_executor);
    
    // Replay and verify
    auto report = driver.replay_and_verify(replay_source);
    
    // Should detect both divergences
    assert(report.deterministic_match == false);
    assert(report.mismatches.size() == 2);
    
    // Verify both mismatches
    assert(report.mismatches[0].retry_index == 0);
    assert(report.mismatches[1].retry_index == 1);
}

int main() {
    test_replay_matches_original_execution();
    test_replay_detects_outcome_divergence();
    test_replay_does_not_skip_execution_based_on_past_outcomes();
    test_replay_does_not_write_persistence();
    test_replay_driver_has_no_runtime_dependencies();
    test_replay_preserves_retry_chain_structure();
    test_replay_detects_multiple_divergences();
    
    return 0;
}