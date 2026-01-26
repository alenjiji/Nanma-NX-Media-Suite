#include "nx/batch/BatchEngineImpl.h"
#include <iomanip>
#include <sstream>

namespace nx::batch {

BatchPlan BatchEngineImpl::plan_batch(const std::vector<ParsedBatchCommand>& commands) const {
    BatchPlan plan;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        JobId job_id{generate_job_id(i + 1)};
        JobState state = commands[i].valid ? JobState::Planned : JobState::Rejected;
        
        plan.jobs.push_back(BatchJobSummary{
            .job_id = job_id,
            .command = commands[i].command,
            .state = state
        });
    }
    
    return plan;
}

std::string BatchEngineImpl::generate_job_id(size_t index) const {
    std::ostringstream oss;
    oss << "job-" << std::setfill('0') << std::setw(3) << index;
    return oss.str();
}

} // namespace nx::batch