#include "nx/monitor/RealMonitorEngine.h"

namespace nx::monitor {

SystemStatus RealMonitorEngine::status() const {
    return {
        .healthy = true,
        .active_jobs = 0,
        .completed_jobs = 0
    };
}

std::vector<JobSummary> RealMonitorEngine::jobs() const {
    return {};
}

std::optional<JobDetail> RealMonitorEngine::job(const std::string&) const {
    return std::nullopt;
}

std::vector<EngineInfo> RealMonitorEngine::engines() const {
    return {
        {"NX-Convert Pro", "1.0.0", true},
        {"NX-AudioLab", "1.0.0", true},
        {"NX-VideoTrans", "1.0.0", true},
        {"NX-MetaFix", "1.0.0", true},
        {"NX-BatchFlow", "1.0.0", true}
    };
}

EngineVersion RealMonitorEngine::version() const {
    return {
        .name = "NX-Monitor",
        .version = "1.0.0",
        .build_id = "real-engine"
    };
}

} // namespace nx::monitor