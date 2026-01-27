#pragma once

#include "SessionTypes.h"
#include <vector>
#include <optional>

namespace nx::batch {

struct ExecutionNode {
    SessionJobId job_id;                    // REFERENCED: Session job identity
    std::vector<SessionJobId> dependencies; // COPIED: Dependency structure from session
    
    bool operator==(const ExecutionNode& other) const = default;
};

class ExecutionGraph {
public:
    explicit ExecutionGraph(std::vector<ExecutionNode> nodes);
    
    const std::vector<ExecutionNode>& nodes() const;
    std::optional<ExecutionNode> node(const SessionJobId& job_id) const;
    
    size_t node_count() const;
    
    bool operator==(const ExecutionGraph& other) const = default;

private:
    std::vector<ExecutionNode> nodes_;
};

} // namespace nx::batch