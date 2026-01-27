#include "nx/batch/ExecutionCoordinator.h"
#include <algorithm>

namespace nx::batch {

std::vector<JobExecutionSpec> ExecutionCoordinator::prepare_job_specs(const BatchPlanSession& session) {
    std::vector<JobExecutionSpec> specs;
    
    for (const auto& job : session.jobs()) {
        specs.push_back(JobExecutionSpec::create(
            ComponentType::Convert,  // Default to Convert for Phase 9
            job.command,
            job.arguments
        ));
    }
    
    return specs;
}

std::optional<JobExecutionSpec> ExecutionCoordinator::prepare_job_spec(const BatchPlanSession& session, const SessionJobId& job_id) {
    auto job = session.job(job_id);
    if (!job.has_value()) {
        return std::nullopt;
    }
    
    return JobExecutionSpec::create(
        ComponentType::Convert,  // Default to Convert for Phase 9
        job->command,
        job->arguments
    );
}

} // namespace nx::batch