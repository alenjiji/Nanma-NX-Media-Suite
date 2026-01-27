#pragma once

#include <string>
#include <chrono>

namespace nx::monitor {
    struct SystemStatus;
}

namespace nx::cli::dto {

struct MonitorStatusDto {
    std::string engine_id;
    std::string engine_version;
    std::chrono::system_clock::time_point startup_time;
    bool is_active;
    std::string current_state;
    size_t active_jobs_count;
    size_t completed_jobs_count;
    size_t failed_jobs_count;
    
    explicit MonitorStatusDto(const nx::monitor::SystemStatus& status);
};

} // namespace nx::cli::dto