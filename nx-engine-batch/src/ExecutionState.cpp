#include "nx/batch/ExecutionState.h"
#include "nx/batch/ExecutionGraph.h"
#include <stdexcept>
#include <algorithm>

namespace nx::batch {

// ExecutionJobState implementation

ExecutionJobState ExecutionJobState::create_planned(const SessionJobId& job_id) noexcept {
    return ExecutionJobState{
        .job_id = job_id,
        .current_state = ExecutionState::Planned,
        .execution_result = std::nullopt
    };
}

ExecutionJobState ExecutionJobState::transition_to_running() const {
    if (current_state != ExecutionState::Planned) {
        throw std::logic_error("Invalid transition: can only transition to Running from Planned state");
    }
    
    return ExecutionJobState{
        .job_id = job_id,
        .current_state = ExecutionState::Running,
        .execution_result = std::nullopt
    };
}

ExecutionJobState ExecutionJobState::transition_to_completed(JobExecutionResult result) const {
    if (current_state != ExecutionState::Running) {
        throw std::logic_error("Invalid transition: can only transition to Completed from Running state");
    }
    
    return ExecutionJobState{
        .job_id = job_id,
        .current_state = ExecutionState::Completed,
        .execution_result = std::move(result)
    };
}

ExecutionJobState ExecutionJobState::transition_to_failed(JobExecutionResult result) const {
    if (current_state != ExecutionState::Running) {
        throw std::logic_error("Invalid transition: can only transition to Failed from Running state");
    }
    
    return ExecutionJobState{
        .job_id = job_id,
        .current_state = ExecutionState::Failed,
        .execution_result = std::move(result)
    };
}

bool ExecutionJobState::is_terminal() const noexcept {
    return current_state == ExecutionState::Completed || 
           current_state == ExecutionState::Failed;
}

// ExecutionStateStore implementation

ExecutionStateStore::ExecutionStateStore(const ExecutionGraph& execution_graph)
    : execution_graph_(&execution_graph) {
    // Initialize all jobs in Planned state with deterministic ordering
    const auto& nodes = execution_graph.nodes();
    job_states_.reserve(nodes.size());
    
    for (const auto& node : nodes) {
        job_states_.push_back(ExecutionJobState::create_planned(node.job_id));
    }
}

const ExecutionJobState& ExecutionStateStore::get_job_state(const SessionJobId& job_id) const {
    size_t index = find_job_index(job_id);
    return job_states_[index];
}

void ExecutionStateStore::update_job_state(const ExecutionJobState& new_state) {
    size_t index = find_job_index(new_state.job_id);
    const auto& current_state = job_states_[index];
    
    // Validate transition is legal
    if (!is_valid_transition(current_state.current_state, new_state.current_state)) {
        throw std::logic_error("Invalid state transition attempted");
    }
    
    // Apply state update atomically
    job_states_[index] = new_state;
}

std::vector<ExecutionJobState> ExecutionStateStore::get_all_states() const {
    return job_states_;  // Return copy for immutability
}

ExecutionStateStore::StateCounts ExecutionStateStore::get_state_counts() const noexcept {
    StateCounts counts;
    
    for (const auto& job_state : job_states_) {
        switch (job_state.current_state) {
            case ExecutionState::Planned:
                counts.planned_count++;
                break;
            case ExecutionState::Running:
                counts.running_count++;
                break;
            case ExecutionState::Completed:
                counts.completed_count++;
                break;
            case ExecutionState::Failed:
                counts.failed_count++;
                break;
        }
    }
    
    return counts;
}

size_t ExecutionStateStore::total_job_count() const noexcept {
    return job_states_.size();
}

bool ExecutionStateStore::all_jobs_terminal() const noexcept {
    return std::all_of(job_states_.begin(), job_states_.end(),
                       [](const ExecutionJobState& state) {
                           return state.is_terminal();
                       });
}

size_t ExecutionStateStore::find_job_index(const SessionJobId& job_id) const {
    auto it = std::find_if(job_states_.begin(), job_states_.end(),
                           [&job_id](const ExecutionJobState& state) {
                               return state.job_id == job_id;
                           });
    
    if (it == job_states_.end()) {
        throw std::out_of_range("Job ID not found in execution state store");
    }
    
    return static_cast<size_t>(std::distance(job_states_.begin(), it));
}

bool ExecutionStateStore::is_valid_transition(ExecutionState from, ExecutionState to) noexcept {
    // Define allowed transitions explicitly
    switch (from) {
        case ExecutionState::Planned:
            return to == ExecutionState::Running;
        case ExecutionState::Running:
            return to == ExecutionState::Completed || to == ExecutionState::Failed;
        case ExecutionState::Completed:
        case ExecutionState::Failed:
            return false;  // Terminal states - no further transitions allowed
    }
    return false;
}

const ExecutionGraph& ExecutionStateStore::get_execution_graph() const noexcept {
    return *execution_graph_;
}

} // namespace nx::batch