#pragma once

#include "MonitorEngine.h"

namespace nx::monitor {

class RealMonitorEngine : public MonitorEngine {
public:
    SystemStatus status() const override;
    std::vector<JobSummary> jobs() const override;
    std::optional<JobDetail> job(const std::string& job_id) const override;
    std::vector<EngineInfo> engines() const override;
    EngineVersion version() const override;
};

} // namespace nx::monitor