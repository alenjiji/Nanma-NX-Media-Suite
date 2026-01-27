#include "nx/batch/ExecutionGraph.h"
#include <algorithm>

namespace nx::batch {

ExecutionGraph::ExecutionGraph(std::vector<ExecutionNode> nodes)
    : nodes_(std::move(nodes)) {
    build_lookup_index();
}

const std::vector<ExecutionNode>& ExecutionGraph::nodes() const {
    return nodes_;
}

std::optional<ExecutionNode> ExecutionGraph::node(const SessionJobId& job_id) const {
    auto it = job_id_to_index_.find(job_id);
    if (it != job_id_to_index_.end()) {
        return nodes_[it->second];
    }
    return std::nullopt;
}

std::optional<JobExecutionSpec> ExecutionGraph::get_spec(const SessionJobId& job_id) const {
    auto it = job_id_to_index_.find(job_id);
    if (it != job_id_to_index_.end()) {
        return nodes_[it->second].spec;
    }
    return std::nullopt;
}

std::vector<JobExecutionSpec> ExecutionGraph::get_all_specs() const {
    std::vector<JobExecutionSpec> specs;
    specs.reserve(nodes_.size());
    
    for (const auto& node : nodes_) {
        specs.push_back(node.spec);
    }
    
    return specs;
}

size_t ExecutionGraph::node_count() const {
    return nodes_.size();
}

void ExecutionGraph::build_lookup_index() {
    job_id_to_index_.clear();
    for (size_t i = 0; i < nodes_.size(); ++i) {
        job_id_to_index_[nodes_[i].job_id] = i;
    }
}

} // namespace nx::batch