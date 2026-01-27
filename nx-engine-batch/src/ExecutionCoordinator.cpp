#include "nx/batch/ExecutionCoordinator.h"
#include <algorithm>

namespace nx::batch {

std::vector<JobExecutionSpec> ExecutionCoordinator::prepare_job_specs(const BatchPlanSession& session) {
    std::vector<JobExecutionSpec> specs;
    
    for (const auto& job : session.jobs()) {
        specs.push_back(JobExecutionSpec{
            .job_id = job.job_id,       // REFERENCED: Job identity from session
            .command = job.command,     // COPIED: Command string from session
            .arguments = job.arguments  // COPIED: Arguments from session
        });
    }
    
    return specs;
}

std::optional<JobExecutionSpec> ExecutionCoordinator::prepare_job_spec(const BatchPlanSession& session, const SessionJobId& job_id) {
    auto job = session.job(job_id);
    if (!job.has_value()) {
        return std::nullopt;
    }
    
    return JobExecutionSpec{
        .job_id = job->job_id,       // REFERENCED: Job identity from session
        .command = job->command,     // COPIED: Command string from session
        .arguments = job->arguments  // COPIED: Arguments from session
    };
}

} // namespace nx::batch