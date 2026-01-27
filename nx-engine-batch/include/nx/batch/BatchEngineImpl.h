#pragma once

#include "BatchEngine.h"

namespace nx::batch {

class BatchEngineImpl : public BatchEngine {
public:
    BatchPlan plan_batch(const std::vector<ParsedBatchCommand>& commands) const override;
    BatchPlanSession create_session(const std::vector<ParsedBatchCommand>& commands) const override;
    ExecutionInput prepare_execution(const BatchPlanSession& session) const override;
    ExecutionGraph create_execution_graph(const BatchPlanSession& session) const override;

private:
    std::string generate_job_id(size_t index) const;
    SessionId generate_session_id(const std::vector<ParsedBatchCommand>& commands) const;
};

} // namespace nx::batch