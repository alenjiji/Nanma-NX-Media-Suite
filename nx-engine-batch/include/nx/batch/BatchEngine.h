#pragma once

#include "BatchTypes.h"
#include "BatchPlanSession.h"
#include "ExecutionInput.h"
#include "ExecutionGraph.h"

namespace nx::batch {

class BatchEngine {
public:
    virtual ~BatchEngine() = default;

    virtual BatchPlan plan_batch(const std::vector<ParsedBatchCommand>& commands) const = 0;
    virtual BatchPlanSession create_session(const std::vector<ParsedBatchCommand>& commands) const = 0;
    virtual ExecutionInput prepare_execution(const BatchPlanSession& session) const = 0;
    virtual ExecutionGraph create_execution_graph(const BatchPlanSession& session) const = 0;
};

} // namespace nx::batch