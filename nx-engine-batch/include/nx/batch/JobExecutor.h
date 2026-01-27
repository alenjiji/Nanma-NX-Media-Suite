#pragma once

#include "JobExecutionSpec.h"
#include "JobExecutionResult.h"

namespace nx::batch {

/**
 * Abstract interface for executing individual jobs
 * 
 * ISOLATION CONSTRAINTS:
 * - Execution cannot observe sibling jobs to prevent hidden dependencies
 * - Execution cannot mutate session or graph to maintain immutability
 * - Execution operates on isolated job specification only
 * 
 * WHY ISOLATION IS REQUIRED:
 * - Sessions represent immutable planning intent
 * - Execution must not create hidden coupling between jobs
 * - Graph structure must remain deterministic and observable
 * - Job execution order must not affect individual job behavior
 * 
 * FORBIDDEN OPERATIONS:
 * - Accessing other jobs in the session
 * - Modifying session state or metadata
 * - Inferring execution context from siblings
 * - Creating dependencies not declared in session
 */
class JobExecutor {
public:
    virtual ~JobExecutor() = default;
    
    /**
     * Execute a single job in isolation
     * 
     * @param spec Immutable job specification containing all execution data
     * @return Result descriptor (no success/failure semantics)
     * 
     * CONSTRAINTS:
     * - Must not access session or graph state
     * - Must not observe other jobs
     * - Must not produce side effects beyond the job itself
     * - Must be deterministic for identical specs
     */
    virtual JobExecutionResult execute_job(const JobExecutionSpec& spec) const = 0;
};

} // namespace nx::batch