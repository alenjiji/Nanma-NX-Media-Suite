#pragma once

#include "BatchTypes.h"

namespace nx::batch {

class BatchEngine {
public:
    virtual ~BatchEngine() = default;

    virtual BatchPlan plan_batch(const std::string& batch_file_path) const = 0;
    virtual std::vector<BatchJobSummary> jobs() const = 0;
    virtual std::optional<BatchJobDetail> job(const JobId& job_id) const = 0;
};

} // namespace nx::batch