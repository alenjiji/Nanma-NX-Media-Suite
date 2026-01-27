#pragma once

#include "SessionTypes.h"
#include "JobExecutionResult.h"
#include <vector>
#include <optional>

namespace nx::batch {

/**
 * Runtime execution state of a single job
 * 
 * ARCHITECTURAL SEPARATION:
 * - Exists ONLY during execution runtime
 * - Completely separate from BatchPlanSession planning artifacts
 * - Ephemeral - destroyed when execution scope ends
 * - Deterministic - same inputs produce identical state progressions
 * 
 * STATE IMMUTABILITY:
 * - Once transitioned OUT of a state, that transition is irreversible
 * - No retries, no state reversions, no partial states
 * - Linear progression only: Planned → Running → (Completed|Failed)
 */
enum class ExecutionState {
    Planned,    // Initial state - job ready for execution
    Running,    // Job is currently executing
    Completed,  // Job finished successfully
    Failed      // Job finished with failure
};

/**
 * Couples job identity with its current runtime execution state
 * 
 * OWNERSHIP MODEL:
 * - Value type - no shared ownership semantics
 * - Immutable once created - state changes create new instances
 * - References SessionJobId but NOT BatchPlanSession directly
 * 
 * LIFECYCLE:
 * - Created from ExecutionGraph during execution initialization
 * - Progresses through states via explicit transitions only
 * - Contains execution result only when in terminal states
 */
struct ExecutionJobState {
    SessionJobId job_id;                                    // REFERENCED: Job identity from planning
    ExecutionState current_state;                           // OWNED: Current runtime state
    std::optional<JobExecutionResult> execution_result;     // OWNED: Result when Completed/Failed
    
    /**
     * Create initial job state in Planned state
     * 
     * @param job_id Job identity from planning phase
     * @return ExecutionJobState in Planned state with no result
     */
    static ExecutionJobState create_planned(const SessionJobId& job_id) noexcept;
    
    /**
     * Transition to Running state
     * 
     * PRECONDITION: current_state must be Planned
     * POSTCONDITION: current_state becomes Running, execution_result remains empty
     * 
     * @return New ExecutionJobState in Running state
     * @throws std::logic_error if current state is not Planned
     */
    ExecutionJobState transition_to_running() const;
    
    /**
     * Transition to Completed state with result
     * 
     * PRECONDITION: current_state must be Running
     * POSTCONDITION: current_state becomes Completed, execution_result is set
     * 
     * @param result Execution result for completed job
     * @return New ExecutionJobState in Completed state with result
     * @throws std::logic_error if current state is not Running
     */
    ExecutionJobState transition_to_completed(JobExecutionResult result) const;
    
    /**
     * Transition to Failed state with result
     * 
     * PRECONDITION: current_state must be Running
     * POSTCONDITION: current_state becomes Failed, execution_result is set
     * 
     * @param result Execution result for failed job
     * @return New ExecutionJobState in Failed state with result
     * @throws std::logic_error if current state is not Running
     */
    ExecutionJobState transition_to_failed(JobExecutionResult result) const;
    
    /**
     * Check if job is in terminal state (Completed or Failed)
     * 
     * @return true if state is Completed or Failed, false otherwise
     */
    bool is_terminal() const noexcept;
    
    bool operator==(const ExecutionJobState& other) const = default;
};

/**
 * In-memory container for all execution job states during runtime
 * 
 * ARCHITECTURAL CONSTRAINTS:
 * - In-memory only - no persistence, no checkpoints, no resume capability
 * - Deterministic iteration order derived from ExecutionGraph structure
 * - Owns all ExecutionJobState instances for the execution session
 * - Constructed from ExecutionGraph, destroyed when execution ends
 * 
 * DETERMINISM GUARANTEES:
 * - Same ExecutionGraph → same initial state layout
 * - Stable iteration order across platforms and runs
 * - No hash-based containers, no environment-dependent ordering
 * - No time-dependent or random behavior
 * 
 * MONITOR INTEGRATION:
 * - Provides read-only snapshots for monitoring
 * - Monitor cannot trigger state transitions
 * - Monitor cannot modify execution state
 * - Monitor observations do not affect execution determinism
 */
class ExecutionStateStore {
public:
    /**
     * Construct execution state store from execution graph
     * 
     * INITIALIZATION:
     * - All jobs start in Planned state
     * - Iteration order matches ExecutionGraph node order
     * - No execution results initially
     * 
     * @param execution_graph Graph defining job structure and dependencies
     */
    explicit ExecutionStateStore(const ExecutionGraph& execution_graph);
    
    /**
     * Get current state of specific job
     * 
     * @param job_id Job to query
     * @return Current ExecutionJobState for the job
     * @throws std::out_of_range if job_id not found
     */
    const ExecutionJobState& get_job_state(const SessionJobId& job_id) const;
    
    /**
     * Update job state with new state
     * 
     * TRANSITION VALIDATION:
     * - Validates transition is legal before applying
     * - Fails fast on invalid transitions
     * - Updates are atomic - either succeed completely or fail completely
     * 
     * @param new_state New state to apply
     * @throws std::logic_error if transition is invalid
     * @throws std::out_of_range if job_id not found
     */
    void update_job_state(const ExecutionJobState& new_state);
    
    /**
     * Get all job states in deterministic order
     * 
     * ORDERING GUARANTEE:
     * - Order matches ExecutionGraph node order
     * - Stable across multiple calls
     * - Platform-independent ordering
     * 
     * @return Vector of all ExecutionJobState instances
     */
    std::vector<ExecutionJobState> get_all_states() const;
    
    /**
     * Get aggregate state counts for monitoring
     * 
     * @return Counts of jobs in each state
     */
    struct StateCounts {
        size_t planned_count = 0;
        size_t running_count = 0;
        size_t completed_count = 0;
        size_t failed_count = 0;
        
        bool operator==(const StateCounts& other) const = default;
    };
    
    StateCounts get_state_counts() const noexcept;
    
    /**
     * Get total number of jobs in execution
     * 
     * @return Total job count
     */
    size_t total_job_count() const noexcept;
    
    /**
     * Check if all jobs are in terminal states
     * 
     * @return true if all jobs are Completed or Failed
     */
    bool all_jobs_terminal() const noexcept;

private:
    std::vector<ExecutionJobState> job_states_;  // OWNED: All job states in deterministic order
    
    // Find job state index by job_id
    size_t find_job_index(const SessionJobId& job_id) const;
    
    // Validate state transition is legal
    static bool is_valid_transition(ExecutionState from, ExecutionState to) noexcept;
};

/**
 * Read-only snapshot of execution state for monitoring
 * 
 * MONITOR SEPARATION:
 * - Monitor receives immutable snapshots only
 * - Monitor cannot trigger state changes
 * - Monitor cannot delay or influence execution
 * - Monitor observations are passive only
 * 
 * WHY SNAPSHOTS:
 * - Prevents Monitor from holding mutable references
 * - Ensures execution state ownership remains with Execution Engine
 * - Maintains deterministic execution independent of monitoring
 */
struct ExecutionStateSnapshot {
    SessionId session_id;                           // REFERENCED: Session identity for correlation
    std::vector<ExecutionJobState> job_states;      // COPIED: Immutable snapshot of all states
    ExecutionStateStore::StateCounts state_counts;  // COPIED: Aggregate counts at snapshot time
    
    bool operator==(const ExecutionStateSnapshot& other) const = default;
};

/**
 * State transition event for monitoring
 * 
 * MONITORING CONTRACT:
 * - Emitted after successful state transitions
 * - Contains only the transition information, not execution details
 * - Monitor receives these as read-only notifications
 * - Monitor cannot influence future transitions based on these events
 */
struct StateTransitionEvent {
    SessionJobId job_id;            // REFERENCED: Job that transitioned
    ExecutionState previous_state;  // COPIED: State before transition
    ExecutionState new_state;       // COPIED: State after transition
    
    bool operator==(const StateTransitionEvent& other) const = default;
};

} // namespace nx::batch