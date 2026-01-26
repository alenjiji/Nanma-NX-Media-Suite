#pragma once

#include "BatchEngine.h"

namespace nx::batch {

class BatchEngineImpl : public BatchEngine {
public:
    BatchPlan plan_batch(const std::string& batch_file_path) const override;
    std::vector<BatchJobSummary> jobs() const override;
    std::optional<BatchJobDetail> job(const JobId& job_id) const override;
};

} // namespace nx::batch