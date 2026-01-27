#include "nx/batch/ExecutionInput.h"
#include <algorithm>

namespace nx::batch {

ExecutionInput::ExecutionInput(SessionId session_id, std::vector<ExecutionJobRef> job_refs)
    : session_id_(std::move(session_id)), job_refs_(std::move(job_refs)) {
}

SessionId ExecutionInput::session_id() const {
    return session_id_;
}

const std::vector<ExecutionJobRef>& ExecutionInput::job_refs() const {
    return job_refs_;
}

std::optional<ExecutionJobRef> ExecutionInput::job_ref(const SessionJobId& job_id) const {
    auto it = std::find_if(job_refs_.begin(), job_refs_.end(),
        [&job_id](const ExecutionJobRef& ref) {
            return ref.job_id == job_id;
        });
    
    if (it != job_refs_.end()) {
        return *it;
    }
    
    return std::nullopt;
}

size_t ExecutionInput::job_count() const {
    return job_refs_.size();
}

} // namespace nx::batch