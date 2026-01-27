#include "nx/monitor/NullMonitorEngine.h"

namespace nx::monitor {

SystemStatus NullMonitorEngine::status() const {
    return {
        .healthy = true,
        .active_jobs = 0,
        .completed_jobs = 0
    };
}

std::vector<JobSummary> NullMonitorEngine::jobs() const {
    return {};
}

std::optional<JobDetail> NullMonitorEngine::job(const std::string& /* job_id */) const {
    return std::nullopt;
}

std::vector<EngineInfo> NullMonitorEngine::engines() const {
    return {
        {"NX-Convert Pro", "1.0.0", true},
        {"NX-AudioLab", "1.0.0", true},
        {"NX-VideoTrans", "1.0.0", true},
        {"NX-MetaFix", "1.0.0", true},
        {"NX-BatchFlow", "1.0.0", true}
    };
}

EngineVersion NullMonitorEngine::version() const {
    return {
        .name = "NX-Monitor",
        .version = "0.1.0",
        .build_id = "null-engine"
    };
}

} // namespace nx::monitor