#pragma once

#include "BatchEngine.h"

namespace nx::batch {

class BatchEngineImpl : public BatchEngine {
public:
    BatchPlan plan_batch(const std::vector<ParsedBatchCommand>& commands) const override;

private:
    std::string generate_job_id(size_t index) const;
};

} // namespace nx::batch