#include "nx/batch/BatchPlanSession.h"
#include <algorithm>

namespace nx::batch {

BatchPlanSession::BatchPlanSession(SessionId session_id, std::vector<SessionJobDescriptor> jobs)
    : session_id_(std::move(session_id)), jobs_(std::move(jobs)) {
}

SessionId BatchPlanSession::id() const {
    return session_id_;
}

const std::vector<SessionJobDescriptor>& BatchPlanSession::jobs() const {
    return jobs_;
}

std::optional<SessionJobDescriptor> BatchPlanSession::job(const SessionJobId& job_id) const {
    auto it = std::find_if(jobs_.begin(), jobs_.end(),
        [&job_id](const SessionJobDescriptor& desc) {
            return desc.job_id == job_id;
        });
    
    if (it != jobs_.end()) {
        return *it;
    }
    
    return std::nullopt;
}

size_t BatchPlanSession::job_count() const {
    return jobs_.size();
}

} // namespace nx::batch