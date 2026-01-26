#pragma once

#include "BatchTypes.h"

namespace nx::batch {

class BatchEngine {
public:
    virtual ~BatchEngine() = default;

    virtual BatchPlan plan_batch(const std::vector<ParsedBatchCommand>& commands) const = 0;
};

} // namespace nx::batch