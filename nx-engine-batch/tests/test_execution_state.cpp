#include "nx/batch/ExecutionState.h"
#include "nx/batch/ExecutionGraph.h"
#include "nx/batch/BatchEngineImpl.h"
#include <cassert>
#include <stdexcept>

using namespace nx::batch;

void test_execution_state_transitions() {
    SessionJobId job_id{SessionId{"test-session"}, JobId{"job-001"}};
    
    // Test initial state creation
    auto planned_state = ExecutionJobState::create_planned(job_id);
    assert(planned_state.job_id == job_id);
    assert(planned_state.current_state == ExecutionState::Planned);
    assert(!planned_state.execution_result.has_value());
    assert(!planned_state.is_terminal());
    
    // Test Planned → Running transition
    auto running_state = planned_state.transition_to_running();
    assert(running_state.job_id == job_id);
    assert(running_state.current_state == ExecutionState::Running);
    assert(!running_state.execution_result.has_value());
    assert(!running_state.is_terminal());
    
    // Test Running → Completed transition
    JobExecutionResult success_result{job_id, true, "Success"};
    auto completed_state = running_state.transition_to_completed(success_result);
    assert(completed_state.job_id == job_id);
    assert(completed_state.current_state == ExecutionState::Completed);
    assert(completed_state.execution_result.has_value());
    assert(completed_state.is_terminal());
    
    // Test Running → Failed transition
    JobExecutionResult failure_result{job_id, false, "Failed"};
    auto failed_state = running_state.transition_to_failed(failure_result);
    assert(failed_state.job_id == job_id);
    assert(failed_state.current_state == ExecutionState::Failed);
    assert(failed_state.execution_result.has_value());
    assert(failed_state.is_terminal());
}

void test_invalid_state_transitions() {
    SessionJobId job_id{SessionId{"test-session"}, JobId{"job-001"}};
    JobExecutionResult result{job_id, true, "Result"};
    
    auto planned_state = ExecutionJobState::create_planned(job_id);
    auto running_state = planned_state.transition_to_running();
    auto completed_state = running_state.transition_to_completed(result);
    
    // Test invalid transitions throw exceptions
    try {
        planned_state.transition_to_completed(result);  // Planned → Completed (invalid)
        assert(false && "Should have thrown exception");
    } catch (const std::logic_error&) {
        // Expected
    }
    
    try {
        planned_state.transition_to_failed(result);  // Planned → Failed (invalid)
        assert(false && "Should have thrown exception");
    } catch (const std::logic_error&) {
        // Expected
    }
    
    try {
        completed_state.transition_to_running();  // Completed → Running (invalid)
        assert(false && "Should have thrown exception");
    } catch (const std::logic_error&) {
        // Expected
    }
}

void test_execution_state_store() {
    // Create execution graph with multiple jobs
    BatchEngineImpl engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test1.mp4 --output test1.mkv", {"nx", "convert", "--input", "test1.mp4", "--output", "test1.mkv"}, true},
        {"nx audio --input test2.wav --output test2.flac", {"nx", "audio", "--input", "test2.wav", "--output", "test2.flac"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto execution_graph = engine.create_execution_graph(session);
    
    // Create execution state store
    ExecutionStateStore state_store(execution_graph);
    
    // Verify initial state
    assert(state_store.total_job_count() == 2);
    assert(!state_store.all_jobs_terminal());
    
    auto initial_counts = state_store.get_state_counts();
    assert(initial_counts.planned_count == 2);
    assert(initial_counts.running_count == 0);
    assert(initial_counts.completed_count == 0);
    assert(initial_counts.failed_count == 0);
    
    // Get all states and verify deterministic ordering
    auto all_states = state_store.get_all_states();
    assert(all_states.size() == 2);
    assert(all_states[0].current_state == ExecutionState::Planned);
    assert(all_states[1].current_state == ExecutionState::Planned);
    
    // Test state transitions
    auto job1_id = all_states[0].job_id;
    auto job2_id = all_states[1].job_id;
    
    // Transition first job to running
    auto job1_running = state_store.get_job_state(job1_id).transition_to_running();
    state_store.update_job_state(job1_running);
    
    auto running_counts = state_store.get_state_counts();
    assert(running_counts.planned_count == 1);
    assert(running_counts.running_count == 1);
    assert(running_counts.completed_count == 0);
    assert(running_counts.failed_count == 0);
    
    // Complete first job
    JobExecutionResult result{job1_id, true, "Success"};
    auto job1_completed = state_store.get_job_state(job1_id).transition_to_completed(result);
    state_store.update_job_state(job1_completed);
    
    // Fail second job
    auto job2_running = state_store.get_job_state(job2_id).transition_to_running();
    state_store.update_job_state(job2_running);
    
    JobExecutionResult failure_result{job2_id, false, "Failed"};
    auto job2_failed = state_store.get_job_state(job2_id).transition_to_failed(failure_result);
    state_store.update_job_state(job2_failed);
    
    // Verify final state
    assert(state_store.all_jobs_terminal());
    
    auto final_counts = state_store.get_state_counts();
    assert(final_counts.planned_count == 0);
    assert(final_counts.running_count == 0);
    assert(final_counts.completed_count == 1);
    assert(final_counts.failed_count == 1);
}

void test_execution_state_snapshot() {
    BatchEngineImpl engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto execution_graph = engine.create_execution_graph(session);
    ExecutionStateStore state_store(execution_graph);
    
    // Create snapshot
    ExecutionStateSnapshot snapshot{
        .session_id = session.id(),
        .job_states = state_store.get_all_states(),
        .state_counts = state_store.get_state_counts()
    };
    
    // Verify snapshot is immutable copy
    assert(snapshot.session_id == session.id());
    assert(snapshot.job_states.size() == 1);
    assert(snapshot.state_counts.planned_count == 1);
    
    // Modify original state store
    auto job_id = snapshot.job_states[0].job_id;
    auto running_state = state_store.get_job_state(job_id).transition_to_running();
    state_store.update_job_state(running_state);
    
    // Verify snapshot unchanged
    assert(snapshot.job_states[0].current_state == ExecutionState::Planned);
    assert(snapshot.state_counts.planned_count == 1);
    assert(snapshot.state_counts.running_count == 0);
}

void test_state_transition_events() {
    SessionJobId job_id{SessionId{"test-session"}, JobId{"job-001"}};
    
    StateTransitionEvent event{
        .job_id = job_id,
        .previous_state = ExecutionState::Planned,
        .new_state = ExecutionState::Running
    };
    
    // Verify event structure
    assert(event.job_id == job_id);
    assert(event.previous_state == ExecutionState::Planned);
    assert(event.new_state == ExecutionState::Running);
    
    // Test equality
    StateTransitionEvent same_event{job_id, ExecutionState::Planned, ExecutionState::Running};
    assert(event == same_event);
    
    StateTransitionEvent different_event{job_id, ExecutionState::Running, ExecutionState::Completed};
    assert(!(event == different_event));
}

int main() {
    test_execution_state_transitions();
    test_invalid_state_transitions();
    test_execution_state_store();
    test_execution_state_snapshot();
    test_state_transition_events();
    
    return 0;
}