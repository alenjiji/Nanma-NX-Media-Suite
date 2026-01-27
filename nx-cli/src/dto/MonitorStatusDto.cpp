#include "MonitorStatusDto.h"

namespace nx::cli::dto {

MonitorStatusDto::MonitorStatusDto(const nx::monitor::SystemStatus& status)
    : engine_id(status.engine_id)
    , engine_version(status.engine_version)
    , startup_time(status.startup_time)
    , is_active(status.is_active)
    , current_state(status.current_state)
    , active_jobs_count(status.active_jobs_count)
    , completed_jobs_count(status.completed_jobs_count)
    , failed_jobs_count(status.failed_jobs_count)
{
}

} // namespace nx::cli::dto