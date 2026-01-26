#include "nx/batch/BatchEngineImpl.h"

namespace nx::batch {

BatchPlan BatchEngineImpl::plan_batch(const std::string& batch_file_path) const {
    (void)batch_file_path;
    return BatchPlan{};
}

std::vector<BatchJobSummary> BatchEngineImpl::jobs() const {
    return {};
}

std::optional<BatchJobDetail> BatchEngineImpl::job(const JobId& job_id) const {
    (void)job_id;
    return std::nullopt;
}

} // namespace nx::batch