#pragma once

#include "BatchPlanSession.h"
#include "JobExecutionSpec.h"
#include "JobExecutionResult.h"
#include "JobExecutor.h"
#include <vector>

namespace nx::batch {

/**
 * Coordinates execution by mapping session jobs to execution specs
 * 
 * RESPONSIBILITY BOUNDARIES:
 * - Converts session jobs to isolated execution specs
 * - Maintains session immutability during execution preparation
 * - Does NOT implement execution logic or scheduling
 * - Does NOT handle dependencies or ordering
 * 
 * WHY SESSION CANNOT BE MUTATED:
 * - Sessions represent immutable planning intent
 * - Execution results belong to execution domain, not planning domain
 * - Session must remain observable in original state
 * - Multiple executions of same session must see identical planning data
 */
class ExecutionCoordinator {
public:
    /**
     * Convert session jobs to isolated execution specifications
     * 
     * @param session Immutable session containing job definitions
     * @return Vector of isolated job specs for execution
     * 
     * GUARANTEES:
     * - Session remains unmodified
     * - Each spec contains complete job information
     * - Specs are independent and isolated
     * - Order matches session job order exactly
     */
    static std::vector<JobExecutionSpec> prepare_job_specs(const BatchPlanSession& session);
    
    /**
     * Extract job specification for single job
     * 
     * @param session Immutable session containing job definitions
     * @param job_id Specific job to extract
     * @return Job spec if found, nullopt otherwise
     */
    static std::optional<JobExecutionSpec> prepare_job_spec(const BatchPlanSession& session, const SessionJobId& job_id);
};

} // namespace nx::batch