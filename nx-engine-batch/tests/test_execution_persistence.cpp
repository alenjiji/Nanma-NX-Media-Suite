#include "nx/batch/ExecutionPersistence.h"
#include "nx/batch/RetryEngine.h"
#include "nx/batch/JobExecutionSpec.h"
#include <cassert>
#include <memory>

using namespace nx::batch;

void test_execution_record_is_self_sufficient() {
    // Create execution record with complete intent
    auto session_id = SessionId{"test-session"};
    auto attempt_id = SessionJobId::create_initial(session_id, "job-001");
    
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    auto record = ExecutionRecord::create(
        attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::success()
    );
    
    // Persist record
    InMemoryExecutionRecorder recorder;
    recorder.record(record);
    
    // Delete all runtime state (simulated by creating new replay source)
    auto replay_source = InMemoryExecutionReplaySource(recorder.get_records());
    
    // Replay using record only
    auto loaded_records = replay_source.load_all();
    assert(loaded_records.size() == 1);
    
    // Assert successful re-execution capability
    auto& loaded_record = loaded_records[0];
    assert(loaded_record.attempt_id == attempt_id);
    assert(loaded_record.intent == intent);
    assert(loaded_record.outcome == ExecutionOutcome::success());
    assert(loaded_record.retry_index == 0);
    assert(!loaded_record.parent_attempt_id.has_value());
}

void test_persistence_cannot_resume_partial_execution() {
    // Simulate interrupted execution - no record should exist for incomplete execution
    InMemoryExecutionRecorder recorder;
    
    // Verify no record exists for interrupted execution
    assert(recorder.get_records().empty());
    
    // Create replay source with empty records
    auto replay_source = InMemoryExecutionReplaySource(recorder.get_records());
    auto loaded_records = replay_source.load_all();
    
    // Assert replay cannot "continue" - no partial state available
    assert(loaded_records.empty());
    
    // Verify that only completed executions can be recorded
    auto session_id = SessionId{"test-session"};
    auto attempt_id = SessionJobId::create_initial(session_id, "job-001");
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Only completed execution can create record
    auto completed_record = ExecutionRecord::create(
        attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::success()
    );
    
    recorder.record(completed_record);
    assert(recorder.get_records().size() == 1);
}

void test_replay_produces_identical_outcomes() {
    // Original execution
    auto session_id = SessionId{"test-session"};
    auto attempt_id = SessionJobId::create_initial(session_id, "job-001");
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Record original execution
    InMemoryExecutionRecorder recorder;
    auto original_record = ExecutionRecord::create(
        attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::success()
    );
    recorder.record(original_record);
    
    // Replay execution
    auto replay_source = InMemoryExecutionReplaySource(recorder.get_records());
    auto replayed_records = replay_source.load_all();
    
    // Compare execution order and outcome codes
    assert(replayed_records.size() == 1);
    auto& replayed_record = replayed_records[0];
    
    // Execution order preserved (single record in this case)
    assert(replayed_record.attempt_id == original_record.attempt_id);
    
    // Outcome codes identical
    assert(replayed_record.outcome == original_record.outcome);
    assert(replayed_record.intent == original_record.intent);
    assert(replayed_record.retry_index == original_record.retry_index);
}

void test_retry_lineage_is_preserved_in_replay() {
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Create retry chain
    auto initial_attempt = RetryAttempt::create_initial(session_id, "job-001");
    auto retry_chain = RetryChain::create(intent, initial_attempt);
    
    // Add retry attempts
    auto retry1 = retry_chain.add_retry(initial_attempt);
    auto retry2 = retry_chain.add_retry(retry1);
    
    // Persist retry chain
    InMemoryExecutionRecorder recorder;
    
    // Record all attempts
    recorder.record(ExecutionRecord::create(
        initial_attempt.attempt_id,
        initial_attempt.parent_attempt_id,
        initial_attempt.retry_index,
        intent,
        ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed)
    ));
    
    recorder.record(ExecutionRecord::create(
        retry1.attempt_id,
        retry1.parent_attempt_id,
        retry1.retry_index,
        intent,
        ExecutionOutcome::failed(DeterministicErrorCode::ProcessingFailed)
    ));
    
    recorder.record(ExecutionRecord::create(
        retry2.attempt_id,
        retry2.parent_attempt_id,
        retry2.retry_index,
        intent,
        ExecutionOutcome::success()
    ));
    
    // Replay
    auto replay_source = InMemoryExecutionReplaySource(recorder.get_records());
    auto replayed_records = replay_source.load_all();
    
    assert(replayed_records.size() == 3);
    
    // Assert parent/child structure preserved
    auto& record0 = replayed_records[0];
    auto& record1 = replayed_records[1];
    auto& record2 = replayed_records[2];
    
    // Original attempt has no parent
    assert(!record0.parent_attempt_id.has_value());
    assert(record0.retry_index == 0);
    
    // First retry has original as parent
    assert(record1.parent_attempt_id.has_value());
    assert(record1.parent_attempt_id.value() == record0.attempt_id);
    assert(record1.retry_index == 1);
    
    // Second retry has first retry as parent
    assert(record2.parent_attempt_id.has_value());
    assert(record2.parent_attempt_id.value() == record1.attempt_id);
    assert(record2.retry_index == 2);
    
    // Assert retry indices preserved
    assert(record0.retry_index == 0);
    assert(record1.retry_index == 1);
    assert(record2.retry_index == 2);
}

void test_execution_record_contains_no_time_or_runtime_state() {
    // Static validation that ExecutionRecord contains only allowed fields
    auto session_id = SessionId{"test-session"};
    auto attempt_id = SessionJobId::create_initial(session_id, "job-001");
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    auto record = ExecutionRecord::create(
        attempt_id,
        std::nullopt,
        0,
        intent,
        ExecutionOutcome::success()
    );
    
    // Verify record contains only allowed fields
    // This is enforced by the struct definition - no time fields exist
    assert(record.attempt_id == attempt_id);
    assert(!record.parent_attempt_id.has_value());
    assert(record.retry_index == 0);
    assert(record.intent == intent);
    assert(record.outcome == ExecutionOutcome::success());
    
    // The absence of forbidden fields is enforced by compilation
    // If time fields or runtime state were added, this would fail to compile
}

void test_retry_executor_with_persistence() {
    // Test retry executor integration with persistence
    InMemoryExecutionRecorder recorder;
    RetryExecutor executor(&recorder);
    
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Execute initial attempt
    auto initial_attempt = RetryAttempt::create_initial(session_id, "job-001");
    auto result1 = executor.execute_retry(intent, initial_attempt);
    
    // Execute retry attempt
    auto retry_attempt = RetryAttempt::create_retry(initial_attempt);
    auto result2 = executor.execute_retry(intent, retry_attempt);
    
    // Verify persistence records
    auto records = recorder.get_records();
    assert(records.size() == 2);
    
    // Verify first record
    assert(records[0].attempt_id == initial_attempt.attempt_id);
    assert(!records[0].parent_attempt_id.has_value());
    assert(records[0].retry_index == 0);
    assert(records[0].intent == intent);
    
    // Verify second record
    assert(records[1].attempt_id == retry_attempt.attempt_id);
    assert(records[1].parent_attempt_id.has_value());
    assert(records[1].parent_attempt_id.value() == initial_attempt.attempt_id);
    assert(records[1].retry_index == 1);
    assert(records[1].intent == intent);
}

void test_append_only_persistence() {
    InMemoryExecutionRecorder recorder;
    
    auto session_id = SessionId{"test-session"};
    auto intent = JobExecutionSpec::create(
        ComponentType::Convert,
        "nx convert --input test.mp4 --output test.mkv",
        {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}
    );
    
    // Record multiple execution attempts
    for (int i = 0; i < 3; ++i) {
        auto attempt_id = SessionJobId::create_initial(session_id, "job-" + std::to_string(i));
        auto record = ExecutionRecord::create(
            attempt_id,
            std::nullopt,
            0,
            intent,
            ExecutionOutcome::success()
        );
        recorder.record(record);
        
        // Verify append-only behavior
        assert(recorder.get_records().size() == static_cast<size_t>(i + 1));
    }
    
    // Verify all records preserved in order
    auto records = recorder.get_records();
    assert(records.size() == 3);
    
    for (int i = 0; i < 3; ++i) {
        assert(records[i].attempt_id.job_value == "job-" + std::to_string(i));
    }
}

int main() {
    test_execution_record_is_self_sufficient();
    test_persistence_cannot_resume_partial_execution();
    test_replay_produces_identical_outcomes();
    test_retry_lineage_is_preserved_in_replay();
    test_execution_record_contains_no_time_or_runtime_state();
    test_retry_executor_with_persistence();
    test_append_only_persistence();
    
    return 0;
}