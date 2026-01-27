#pragma once

#include "SessionTypes.h"
#include "BatchTypes.h"
#include <vector>
#include <optional>

namespace nx::batch {

struct SessionJobDescriptor {
    SessionJobId job_id;
    std::string command;
    std::vector<std::string> arguments;
    std::vector<SessionJobId> dependencies;
    
    bool operator==(const SessionJobDescriptor& other) const = default;
};

class BatchPlanSession {
public:
    explicit BatchPlanSession(SessionId session_id, std::vector<SessionJobDescriptor> jobs);
    
    SessionId id() const;
    const std::vector<SessionJobDescriptor>& jobs() const;
    std::optional<SessionJobDescriptor> job(const SessionJobId& job_id) const;
    
    size_t job_count() const;
    
    bool operator==(const BatchPlanSession& other) const = default;

private:
    SessionId session_id_;
    std::vector<SessionJobDescriptor> jobs_;
};

} // namespace nx::batch