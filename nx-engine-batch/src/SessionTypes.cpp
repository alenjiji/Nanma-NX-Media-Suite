#include "nx/batch/SessionTypes.h"

namespace nx::batch {

SessionJobId SessionJobId::create_initial(SessionId session, std::string job_value) {
    return SessionJobId{
        .session = std::move(session),
        .job_value = std::move(job_value),
        .attempt_index = 0
    };
}

SessionJobId SessionJobId::create_retry(const SessionJobId& previous) {
    return SessionJobId{
        .session = previous.session,
        .job_value = previous.job_value,
        .attempt_index = previous.attempt_index + 1
    };
}

} // namespace nx::batch