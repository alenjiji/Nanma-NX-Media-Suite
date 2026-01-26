#include "nx/monitor/RealMonitorEngine.h"
#include <algorithm>

namespace nx::monitor {

SystemStatus RealMonitorEngine::status() const {
    // All engines are healthy in Phase 1.A (no failure modes implemented)
    bool healthy = true;
    size_t active_jobs = 0;
    size_t completed_jobs = 0;
    
    // No job APIs exist in Phase 1.A engines, so counts remain 0
    
    return {
        .healthy = healthy,
        .active_jobs = active_jobs,
        .completed_jobs = completed_jobs
    };
}

std::vector<JobSummary> RealMonitorEngine::jobs() const {
    // Phase 1.A engines have no job APIs, return empty
    return {};
}

std::optional<JobDetail> RealMonitorEngine::job(const std::string& job_id) const {
    // Phase 1.A engines have no job APIs, return nullopt
    return std::nullopt;
}

std::vector<EngineInfo> RealMonitorEngine::engines() const {
    return {
        {"NX-Convert Pro", "1.0.0", true},
        {"NX-AudioLab", "1.0.0", true},
        {"NX-VideoTrans", "1.0.0", true},
        {"NX-MetaFix", "1.0.0", true}
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