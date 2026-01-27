#include "MonitorStatusDto.h"
#include "nx/monitor/MonitorEngine.h"

namespace nx::cli::dto {

MonitorStatusDto::MonitorStatusDto(const nx::monitor::SystemStatus& status)
    : engine_id("monitor")
    , engine_version("1.0.0")
    , startup_time(std::chrono::system_clock::now())
    , is_active(status.healthy)
    , current_state(status.healthy ? "active" : "inactive")
    , active_jobs_count(status.active_jobs)
    , completed_jobs_count(status.completed_jobs)
    , failed_jobs_count(0)
{
}

} // namespace nx::cli::dto