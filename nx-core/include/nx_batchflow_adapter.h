#pragma once

#include "nx_batchflow_dag.h"
#include "result.h"
#include <string>

namespace nx::batchflow {

/**
 * JobResult - Canonical output from adapter execution
 * 
 * Determinism guarantee: Identical JobNode inputs produce bit-identical JobResult outputs
 * across different executions, processes, and hardware configurations.
 */
struct JobResult {
    enum class Status {
        Success,        // Engine operation completed successfully
        EngineFailure,  // Engine reported failure (not adapter failure)
        InvalidInput    // JobNode validation failed
    };
    
    Status status;
    std::string engine_message;     // Verbatim engine output/error message
    std::vector<std::string> output_artifacts;  // Engine-produced artifact paths
    
    // Deterministic comparison for replay verification
    bool operator==(const JobResult& other) const noexcept = default;
};

/**
 * ExecutionAdapter - Pure binding interface for engine invocation
 * 
 * Constitutional constraints:
 * - Stateless: No member variables, no caching, no persistent state
 * - Deterministic: Same JobNode always produces identical JobResult
 * - Minimal: Only converts JobNode -> engine call -> JobResult
 * - Sealed: Cannot access engine internals or orchestration logic
 */
class ExecutionAdapter {
public:
    /**
     * Execute single engine operation from JobNode
     * 
     * Adapter Law compliance:
     * - Input: JobNode ONLY (no global state, context, or environment)
     * - Output: JobResult ONLY (no side effects, file writes, or mutations)
     * - Behavior: Stateless, deterministic, replay-safe
     * 
     * Error propagation rules:
     * - Engine failures -> JobResult::EngineFailure with verbatim engine message
     * - Invalid JobNode -> JobResult::InvalidInput with validation details
     * - Adapter bugs -> Exception (adapter implementation error)
     * 
     * Determinism guarantee:
     * Given identical JobNode input, this method MUST produce bit-identical
     * JobResult output across all executions, regardless of:
     * - Process restart
     * - Hardware configuration  
     * - Execution timing
     * - Previous adapter calls
     */
    virtual JobResult execute(const JobNode& job_node) const = 0;
    
    /**
     * Get adapter identifier for audit trails
     * 
     * Must return constant string identifying this adapter implementation.
     * Used for replay verification and debugging only.
     */
    virtual std::string adapter_id() const noexcept = 0;
    
    /**
     * Validate JobNode structural correctness
     * 
     * Checks ONLY:
     * - Required fields present
     * - Parameter format validity
     * - Type safety
     * - Engine API compatibility
     * 
     * Does NOT check:
     * - Media semantics
     * - Quality implications
     * - Processing strategy
     * - File existence
     */
    virtual Result<void> validate_job_node(const JobNode& job_node) const noexcept = 0;
    
    virtual ~ExecutionAdapter() = default;
    
protected:
    // Prevent instantiation of base class
    ExecutionAdapter() = default;
    
private:
    // Explicitly deleted to enforce statelessness
    ExecutionAdapter(const ExecutionAdapter&) = delete;
    ExecutionAdapter& operator=(const ExecutionAdapter&) = delete;
    ExecutionAdapter(ExecutionAdapter&&) = delete;
    ExecutionAdapter& operator=(ExecutionAdapter&&) = delete;
};

} // namespace nx::batchflow