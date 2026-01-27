#include "nx/batch/DeterministicExecutionEngine.h"
#include "determinism_guards.h"
#include <algorithm>
#include <stdexcept>

namespace nx::batch {

// DeterministicExecutionEngine implementation

DeterministicExecutionEngine::DeterministicExecutionEngine(
    const ExecutionGraph& execution_graph,
    std::shared_ptr<JobExecutor> job_executor,
    ExecutionEngineObserver* observer)
    : state_store_(execution_graph)
    , execution_order_(compute_execution_order(execution_graph))
    , job_executor_(std::move(job_executor))
    , observer_(observer)
    , current_execution_index_(0)
    , session_id_(execution_graph.nodes().empty() ? SessionId{""} : execution_graph.nodes()[0].job_id.session) {
    
    NX_DETERMINISTIC_FUNCTION;
    nx::core::DeterminismGuard::assert_no_time_access();
    nx::core::DeterminismGuard::assert_no_random_access();
    
    if (!job_executor_) {
        throw std::invalid_argument("JobExecutor cannot be null");
    }
}

DeterministicExecutionEngine::ExecutionResult DeterministicExecutionEngine::execute_all_jobs() {
    NX_DETERMINISTIC_FUNCTION;
    nx::core::DeterminismGuard::assert_no_time_access();
    
    bool all_completed = true;
    size_t jobs_executed = 0;
    
    // Execute jobs in deterministic order
    for (const auto& job_id : execution_order_) {
        bool job_succeeded = execute_single_job(job_id);
        jobs_executed++;
        
        if (!job_succeeded) {
            // Halt execution deterministically on failure
            all_completed = false;
            notify_execution_halt(job_id);
            break;
        }
    }
    
    if (all_completed) {
        notify_execution_complete();
    }
    
    return ExecutionResult{
        .all_jobs_completed = all_completed,
        .jobs_executed = jobs_executed,
        .trace = execution_trace_,
        .final_state = get_current_state()
    };
}

ExecutionStateSnapshot DeterministicExecutionEngine::get_current_state() const {
    return ExecutionStateSnapshot{
        .session_id = session_id_,
        .job_states = state_store_.get_all_states(),
        .state_counts = state_store_.get_state_counts()
    };
}

std::vector<ExecutionTraceRecord> DeterministicExecutionEngine::get_execution_trace() const {
    return execution_trace_;
}

std::vector<SessionJobId> DeterministicExecutionEngine::compute_execution_order(
    const ExecutionGraph& execution_graph) {
    
    NX_DETERMINISTIC_FUNCTION;
    nx::core::DeterminismGuard::assert_no_random_access();
    
    // Use graph node order directly for deterministic execution
    // In Phase 8.2, we use the order from ExecutionGraph nodes
    // Future phases may implement topological sorting for dependencies
    
    std::vector<SessionJobId> order;
    const auto& nodes = execution_graph.nodes();
    order.reserve(nodes.size());
    
    for (const auto& node : nodes) {
        order.push_back(node.job_id);
    }
    
    return order;
}

bool DeterministicExecutionEngine::execute_single_job(const SessionJobId& job_id) {
    NX_DETERMINISTIC_FUNCTION;
    
    // Phase 1: State Transition Planned → Running
    const auto& current_state = state_store_.get_job_state(job_id);
    if (current_state.current_state != ExecutionState::Planned) {
        throw std::logic_error("Job not in Planned state for execution");
    }
    
    auto running_state = current_state.transition_to_running();
    state_store_.update_job_state(running_state);
    record_state_transition(job_id, ExecutionState::Planned, ExecutionState::Running);
    
    // Phase 2: Job Execution
    // PHASE 9 BRIDGE: Map from SessionJobId to JobExecutionSpec
    auto spec_opt = state_store_.get_execution_graph().get_spec(job_id);
    if (!spec_opt) {
        throw std::logic_error("JobExecutionSpec not found for SessionJobId");
    }
    
    JobExecutionResult execution_result = job_executor_->execute_job(spec_opt.value());
    
    // Phase 3: State Transition Running → Terminal
    const auto& running_job_state = state_store_.get_job_state(job_id);
    ExecutionJobState terminal_state;
    ExecutionState terminal_state_enum;
    
    if (execution_result.success) {
        terminal_state = running_job_state.transition_to_completed(execution_result);
        terminal_state_enum = ExecutionState::Completed;
    } else {
        terminal_state = running_job_state.transition_to_failed(execution_result);
        terminal_state_enum = ExecutionState::Failed;
    }
    
    state_store_.update_job_state(terminal_state);
    record_state_transition(job_id, ExecutionState::Running, terminal_state_enum);
    
    // Phase 4: Propagation (monitoring events already emitted)
    return execution_result.success;
}

void DeterministicExecutionEngine::record_state_transition(
    const SessionJobId& job_id,
    ExecutionState previous_state,
    ExecutionState new_state) {
    
    ExecutionTraceRecord trace_record{
        .execution_index = current_execution_index_++,
        .job_id = job_id,
        .previous_state = previous_state,
        .new_state = new_state
    };
    
    execution_trace_.push_back(trace_record);
    
    if (observer_) {
        observer_->observe_state_transition(trace_record);
    }
}

void DeterministicExecutionEngine::notify_execution_complete() {
    if (observer_) {
        auto state_counts = state_store_.get_state_counts();
        observer_->observe_execution_complete(
            session_id_,
            state_store_.total_job_count(),
            state_counts.completed_count
        );
    }
}

void DeterministicExecutionEngine::notify_execution_halt(const SessionJobId& failed_job_id) {
    if (observer_) {
        observer_->observe_execution_halt(
            session_id_,
            failed_job_id,
            current_execution_index_ - 1  // Index of the failed transition
        );
    }
}

// StubJobExecutor implementation

JobExecutionResult StubJobExecutor::execute_job(const JobExecutionSpec& spec) const {
    NX_DETERMINISTIC_FUNCTION;
    nx::core::DeterminismGuard::assert_no_time_access();
    nx::core::DeterminismGuard::assert_no_random_access();
    
    // Deterministic stub execution - always succeeds for Phase 8.2
    // Uses spec hash for deterministic behavior
    return JobExecutionResult{
        .success = true,
        .message = "Stub execution completed successfully",
        .result_token = "stub_result_" + spec.hash.value
    };
}

} // namespace nx::batch