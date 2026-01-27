#include "nx/batch/ExecutionGraph.h"
#include <algorithm>

namespace nx::batch {

ExecutionGraph::ExecutionGraph(std::vector<ExecutionNode> nodes)
    : nodes_(std::move(nodes)) {
}

const std::vector<ExecutionNode>& ExecutionGraph::nodes() const {
    return nodes_;
}

std::optional<ExecutionNode> ExecutionGraph::node(const SessionJobId& job_id) const {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&job_id](const ExecutionNode& node) {
            return node.job_id == job_id;
        });
    
    if (it != nodes_.end()) {
        return *it;
    }
    
    return std::nullopt;
}

size_t ExecutionGraph::node_count() const {
    return nodes_.size();
}

} // namespace nx::batch