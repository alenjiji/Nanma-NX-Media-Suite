#pragma once

#include "SessionTypes.h"
#include <vector>
#include <string>
#include <optional>

namespace nx::batch {

struct ExecutionJobRef {
    SessionJobId job_id;
    
    bool operator==(const ExecutionJobRef& other) const = default;
};

class ExecutionInput {
public:
    explicit ExecutionInput(SessionId session_id, std::vector<ExecutionJobRef> job_refs);
    
    SessionId session_id() const;
    const std::vector<ExecutionJobRef>& job_refs() const;
    std::optional<ExecutionJobRef> job_ref(const SessionJobId& job_id) const;
    
    size_t job_count() const;
    
    bool operator==(const ExecutionInput& other) const = default;

private:
    SessionId session_id_;
    std::vector<ExecutionJobRef> job_refs_;
};

} // namespace nx::batch