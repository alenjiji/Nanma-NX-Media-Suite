#include "nx/batch/DeterministicExecutionEngine.h"
#include "nx/batch/BatchEngineImpl.h"
#include <cassert>
#include <memory>
#include <vector>

using namespace nx::batch;

// Test observer for monitoring validation
class TestExecutionObserver : public ExecutionEngineObserver {
public:
    std::vector<ExecutionTraceRecord> observed_transitions;
    std::vector<SessionId> completed_sessions;
    std::vector<SessionJobId> halted_jobs;
    
    void observe_state_transition(const ExecutionTraceRecord& trace_record) override {
        observed_transitions.push_back(trace_record);
    }
    
    void observe_execution_complete(const SessionId& session_id, 
                                  size_t total_jobs, 
                                  size_t successful_jobs) override {
        completed_sessions.push_back(session_id);
    }
    
    void observe_execution_halt(const SessionId& session_id,
                               const SessionJobId& failed_job_id,
                               size_t execution_index) override {
        halted_jobs.push_back(failed_job_id);
    }
};

// Controllable job executor for testing failure scenarios
class TestJobExecutor : public nx::batch::JobExecutor {
public:
    // Specs that should fail (configured before execution)
    std::vector<JobSpecHash> fail_specs;

    // Observational state (allowed to change in const method)
    mutable std::vector<JobSpecHash> executed_specs;

    JobExecutionResult
    execute_job(const JobExecutionSpec& spec) const override {
        executed_specs.push_back(spec.hash);

        bool should_fail =
            std::find(fail_specs.begin(), fail_specs.end(), spec.hash) != fail_specs.end();

        return JobExecutionResult{
            .success = !should_fail,
            .message = should_fail ? "Test failure" : "Test success",
            .result_token = "test_result_" + spec.hash.value
        };
    }
};


void test_deterministic_execution_order() {
    // Create execution graph with multiple jobs
    BatchEngineImpl batch_engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test1.mp4 --output test1.mkv", {"nx", "convert", "--input", "test1.mp4", "--output", "test1.mkv"}, true},
        {"nx audio --input test2.wav --output test2.flac", {"nx", "audio", "--input", "test2.wav", "--output", "test2.flac"}, true},
        {"nx video --input test3.avi --output test3.mp4", {"nx", "video", "--input", "test3.avi", "--output", "test3.mp4"}, true}
    };
    
    auto session = batch_engine.create_session(commands);
    auto execution_graph = batch_engine.create_execution_graph(session);
    
    // Execute multiple times and verify identical order
    std::vector<std::vector<SessionJobId>> execution_orders;
    
    for (int run = 0; run < 3; ++run) {
        auto job_executor = std::make_shared<TestJobExecutor>();
        TestExecutionObserver observer;
        
        DeterministicExecutionEngine engine(execution_graph, job_executor, &observer);
        auto result = engine.execute_all_jobs();
        
        // Verify successful execution
        assert(result.all_jobs_completed);
        assert(result.jobs_executed == 3);
        assert(result.trace.size() == 6);  // 2 transitions per job
        
        // Extract execution order from trace
        std::vector<SessionJobId> order;
        for (const auto& trace : result.trace) {
            if (trace.previous_state == ExecutionState::Planned && 
                trace.new_state == ExecutionState::Running) {
                order.push_back(trace.job_id);
            }
        }
        
        execution_orders.push_back(order);
        
        // Verify observer received all events
        assert(observer.observed_transitions.size() == 6);
        assert(observer.completed_sessions.size() == 1);
        assert(observer.halted_jobs.empty());
    }
    
    // Verify all runs produced identical execution order
    for (size_t i = 1; i < execution_orders.size(); ++i) {
        assert(execution_orders[0] == execution_orders[i]);
    }
}

void test_state_transition_sequences() {
    BatchEngineImpl batch_engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = batch_engine.create_session(commands);
    auto execution_graph = batch_engine.create_execution_graph(session);
    
    auto job_executor = std::make_shared<StubJobExecutor>();
    TestExecutionObserver observer;
    
    DeterministicExecutionEngine engine(execution_graph, job_executor, &observer);
    auto result = engine.execute_all_jobs();
    
    // Verify exact state transition sequence
    assert(result.trace.size() == 2);
    
    // First transition: Planned → Running
    assert(result.trace[0].execution_index == 0);
    assert(result.trace[0].previous_state == ExecutionState::Planned);
    assert(result.trace[0].new_state == ExecutionState::Running);
    
    // Second transition: Running → Completed
    assert(result.trace[1].execution_index == 1);
    assert(result.trace[1].previous_state == ExecutionState::Running);
    assert(result.trace[1].new_state == ExecutionState::Completed);
    
    // Verify final state
    assert(result.final_state.state_counts.planned_count == 0);
    assert(result.final_state.state_counts.running_count == 0);
    assert(result.final_state.state_counts.completed_count == 1);
    assert(result.final_state.state_counts.failed_count == 0);
}

void test_deterministic_failure_halt() {
    BatchEngineImpl batch_engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test1.mp4 --output test1.mkv", {"nx", "convert", "--input", "test1.mp4", "--output", "test1.mkv"}, true},
        {"nx audio --input test2.wav --output test2.flac", {"nx", "audio", "--input", "test2.wav", "--output", "test2.flac"}, true},
        {"nx video --input test3.avi --output test3.mp4", {"nx", "video", "--input", "test3.avi", "--output", "test3.mp4"}, true}
    };
    
    auto session = batch_engine.create_session(commands);
    auto execution_graph = batch_engine.create_execution_graph(session);
    
    // Configure second job to fail
    auto job_executor = std::make_shared<TestJobExecutor>();
    auto all_states = ExecutionStateStore(execution_graph).get_all_states();
    auto spec = execution_graph.get_spec(all_states[1].job_id);
    job_executor->fail_specs.push_back(spec->hash);  // Fail second job
    
    TestExecutionObserver observer;
    DeterministicExecutionEngine engine(execution_graph, job_executor, &observer);
    auto result = engine.execute_all_jobs();
    
    // Verify execution halted after second job failure
    assert(!result.all_jobs_completed);
    assert(result.jobs_executed == 2);  // First job succeeded, second failed, third not executed
    assert(result.trace.size() == 4);   // 2 transitions for first job, 2 for second job
    
    // Verify final state counts
    assert(result.final_state.state_counts.planned_count == 1);   // Third job never started
    assert(result.final_state.state_counts.running_count == 0);
    assert(result.final_state.state_counts.completed_count == 1); // First job completed
    assert(result.final_state.state_counts.failed_count == 1);    // Second job failed
    
    // Verify observer received halt notification
    assert(observer.halted_jobs.size() == 1);
    assert(observer.halted_jobs[0] == all_states[1].job_id);
    assert(observer.completed_sessions.empty());  // No completion due to halt
}

void test_execution_trace_determinism() {
    BatchEngineImpl batch_engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test1.mp4 --output test1.mkv", {"nx", "convert", "--input", "test1.mp4", "--output", "test1.mkv"}, true},
        {"nx audio --input test2.wav --output test2.flac", {"nx", "audio", "--input", "test2.wav", "--output", "test2.flac"}, true}
    };
    
    auto session = batch_engine.create_session(commands);
    auto execution_graph = batch_engine.create_execution_graph(session);
    
    // Execute multiple times and compare traces
    std::vector<std::vector<ExecutionTraceRecord>> traces;
    
    for (int run = 0; run < 3; ++run) {
        auto job_executor = std::make_shared<StubJobExecutor>();
        DeterministicExecutionEngine engine(execution_graph, job_executor);
        auto result = engine.execute_all_jobs();
        traces.push_back(result.trace);
    }
    
    // Verify all traces are identical
    for (size_t i = 1; i < traces.size(); ++i) {
        assert(traces[0] == traces[i]);
    }
    
    // Verify trace structure
    const auto& trace = traces[0];
    assert(trace.size() == 4);  // 2 jobs × 2 transitions each
    
    // Verify execution indices are sequential
    for (size_t i = 0; i < trace.size(); ++i) {
        assert(trace[i].execution_index == i);
    }
}

void test_monitor_integration_isolation() {
    BatchEngineImpl batch_engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = batch_engine.create_session(commands);
    auto execution_graph = batch_engine.create_execution_graph(session);
    
    // Execute with observer
    auto job_executor1 = std::make_shared<StubJobExecutor>();
    TestExecutionObserver observer1;
    DeterministicExecutionEngine engine1(execution_graph, job_executor1, &observer1);
    auto result1 = engine1.execute_all_jobs();
    
    // Execute without observer
    auto job_executor2 = std::make_shared<StubJobExecutor>();
    DeterministicExecutionEngine engine2(execution_graph, job_executor2, nullptr);
    auto result2 = engine2.execute_all_jobs();
    
    // Verify identical execution results regardless of observer presence
    assert(result1.all_jobs_completed == result2.all_jobs_completed);
    assert(result1.jobs_executed == result2.jobs_executed);
    assert(result1.trace == result2.trace);
    assert(result1.final_state.state_counts == result2.final_state.state_counts);
}

void test_execution_state_integration() {
    BatchEngineImpl batch_engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = batch_engine.create_session(commands);
    auto execution_graph = batch_engine.create_execution_graph(session);
    
    auto job_executor = std::make_shared<StubJobExecutor>();
    DeterministicExecutionEngine engine(execution_graph, job_executor);
    
    // Verify initial state
    auto initial_state = engine.get_current_state();
    assert(initial_state.state_counts.planned_count == 1);
    assert(initial_state.state_counts.running_count == 0);
    assert(initial_state.state_counts.completed_count == 0);
    assert(initial_state.state_counts.failed_count == 0);
    
    // Execute and verify final state
    auto result = engine.execute_all_jobs();
    auto final_state = engine.get_current_state();
    
    assert(final_state.state_counts.planned_count == 0);
    assert(final_state.state_counts.running_count == 0);
    assert(final_state.state_counts.completed_count == 1);
    assert(final_state.state_counts.failed_count == 0);
    
    // Verify state snapshot matches result
    assert(result.final_state.state_counts == final_state.state_counts);
}

int main() {
    test_deterministic_execution_order();
    test_state_transition_sequences();
    test_deterministic_failure_halt();
    test_execution_trace_determinism();
    test_monitor_integration_isolation();
    test_execution_state_integration();
    
    return 0;
}