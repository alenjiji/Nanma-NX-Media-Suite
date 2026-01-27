#pragma once

#include "ExecutionState.h"
#include "ExecutionGraph.h"
#include "JobExecutor.h"
#include <vector>
#include <functional>
#include <memory>

namespace nx::batch {

/**
 * Execution trace record for deterministic observability
 * 
 * DETERMINISM GUARANTEE:
 * - Same ExecutionGraph produces identical execution traces
 * - Execution index is stable across runs
 * - No wall-clock time dependencies
 * - Hardware-independent ordering
 */
struct ExecutionTraceRecord {
    size_t execution_index;                     // OWNED: Deterministic execution order
    SessionJobId job_id;                        // REFERENCED: Job identity
    ExecutionState previous_state;              // COPIED: State before transition
    ExecutionState new_state;                   // COPIED: State after transition
    
    bool operator==(const ExecutionTraceRecord& other) const = default;
};

/**
 * Monitor event observer for execution engine
 * 
 * MONITORING SEPARATION:
 * - Observer receives read-only events only
 * - Observer cannot mutate execution state
 * - Observer cannot delay or influence execution
 * - Observer events do not affect determinism
 */
class ExecutionEngineObserver {
public:
    virtual ~ExecutionEngineObserver() = default;
    
    /**
     * Observe job state transition
     * 
     * @param trace_record Immutable execution trace record
     */
    virtual void observe_state_transition(const ExecutionTraceRecord& trace_record) = 0;
    
    /**
     * Observe execution completion
     * 
     * @param session_id Session that completed
     * @param total_jobs Total number of jobs executed
     * @param successful_jobs Number of jobs that completed successfully
     */
    virtual void observe_execution_complete(const SessionId& session_id, 
                                          size_t total_jobs, 
                                          size_t successful_jobs) = 0;
    
    /**
     * Observe execution halt due to failure
     * 
     * @param session_id Session that halted
     * @param failed_job_id Job that caused halt
     * @param execution_index Index where execution halted
     */
    virtual void observe_execution_halt(const SessionId& session_id,
                                       const SessionJobId& failed_job_id,
                                       size_t execution_index) = 0;
};

/**
 * Deterministic execution engine loop
 * 
 * ARCHITECTURAL RESPONSIBILITY:
 * - Drives ExecutionJobState transitions via ExecutionStateStore
 * - Executes jobs in stable, deterministic order derived from ExecutionGraph
 * - Halts deterministically on failure
 * - Emits read-only monitoring events
 * 
 * FORBIDDEN RESPONSIBILITIES:
 * - Defining or modifying job graphs
 * - Creating ExecutionJobState outside legal transitions
 * - Performing retries or recovery
 * - Persisting execution state
 * - Adaptive scheduling or performance optimization
 * 
 * DETERMINISM GUARANTEES:
 * - Same ExecutionGraph → same execution order
 * - Same inputs → same state transition sequences
 * - Same failure points → same halt behavior
 * - Hardware and OS independent execution
 */
class DeterministicExecutionEngine {
public:
    /**
     * Construct execution engine for given graph
     * 
     * INITIALIZATION:
     * - Creates ExecutionStateStore from ExecutionGraph
     * - Establishes deterministic job ordering
     * - All jobs start in Planned state
     * 
     * @param execution_graph Validated execution graph
     * @param job_executor Job execution implementation
     * @param observer Optional monitoring observer (may be nullptr)
     */
    explicit DeterministicExecutionEngine(const ExecutionGraph& execution_graph,
                                         std::shared_ptr<JobExecutor> job_executor,
                                         ExecutionEngineObserver* observer = nullptr);
    
    /**
     * Execute all jobs in deterministic order
     * 
     * EXECUTION LIFECYCLE:
     * 1. Iterate jobs in graph-defined order
     * 2. For each job: Planned → Running → (Completed|Failed)
     * 3. Halt immediately on any failure
     * 4. Emit monitoring events for all transitions
     * 
     * DETERMINISM GUARANTEE:
     * - Execution order is identical across runs
     * - State transitions follow exact sequence
     * - Failure halts at identical point
     * 
     * @return ExecutionResult with final state and trace
     */
    struct ExecutionResult {
        bool all_jobs_completed;                        // OWNED: True if all jobs completed successfully
        size_t jobs_executed;                           // OWNED: Number of jobs that were executed
        std::vector<ExecutionTraceRecord> trace;        // OWNED: Complete execution trace
        ExecutionStateSnapshot final_state;             // OWNED: Final state snapshot
        
        bool operator==(const ExecutionResult& other) const = default;
    };
    
    ExecutionResult execute_all_jobs();
    
    /**
     * Get current execution state snapshot
     * 
     * @return Immutable snapshot of current execution state
     */
    ExecutionStateSnapshot get_current_state() const;
    
    /**
     * Get execution trace so far
     * 
     * @return Vector of all execution trace records
     */
    std::vector<ExecutionTraceRecord> get_execution_trace() const;

private:
    ExecutionStateStore state_store_;                   // OWNED: Execution state management
    std::vector<SessionJobId> execution_order_;         // OWNED: Deterministic job execution order
    std::shared_ptr<JobExecutor> job_executor_;         // REFERENCED: Job execution implementation
    ExecutionEngineObserver* observer_;                 // REFERENCED: Optional monitoring observer
    std::vector<ExecutionTraceRecord> execution_trace_; // OWNED: Complete execution trace
    size_t current_execution_index_;                    // OWNED: Current position in execution
    SessionId session_id_;                              // REFERENCED: Session identity for events
    
    // Establish deterministic execution order from ExecutionGraph
    std::vector<SessionJobId> compute_execution_order(const ExecutionGraph& execution_graph);
    
    // Execute single job through complete lifecycle
    bool execute_single_job(const SessionJobId& job_id);
    
    // Record state transition in trace and notify observer
    void record_state_transition(const SessionJobId& job_id, 
                                ExecutionState previous_state, 
                                ExecutionState new_state);
    
    // Notify observer of execution completion
    void notify_execution_complete();
    
    // Notify observer of execution halt
    void notify_execution_halt(const SessionJobId& failed_job_id);
};

/**
 * Stub job executor for testing and Phase 8.2 validation
 * 
 * DETERMINISTIC BEHAVIOR:
 * - Always succeeds for valid job IDs
 * - Produces identical results across runs
 * - No actual media processing (Phase 8.2 scope)
 */
class StubJobExecutor : public JobExecutor {
public:
    JobExecutionResult execute_job(const JobExecutionSpec& spec) const override;
};

} // namespace nx::batch