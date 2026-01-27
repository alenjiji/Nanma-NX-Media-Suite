#include "MonitorStatusTextSerializer.h"
#include "../dto/MonitorStatusDto.h"
#include <sstream>

namespace nx::cli::serialize {

std::string MonitorStatusTextSerializer::serialize(const dto::MonitorStatusDto& dto) {
    std::ostringstream text;
    
    text << "engine_id=" << dto.engine_id << "\n";
    text << "engine_version=" << dto.engine_version << "\n";
    text << "startup_time=" << dto.startup_time.time_since_epoch().count() << "\n";
    text << "is_active=" << (dto.is_active ? "true" : "false") << "\n";
    text << "current_state=" << dto.current_state << "\n";
    text << "active_jobs_count=" << dto.active_jobs_count << "\n";
    text << "completed_jobs_count=" << dto.completed_jobs_count << "\n";
    text << "failed_jobs_count=" << dto.failed_jobs_count << "\n";
    
    return text.str();
}

} // namespace nx::cli::serialize