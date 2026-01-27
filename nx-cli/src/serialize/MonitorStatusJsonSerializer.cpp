#include "MonitorStatusJsonSerializer.h"
#include "../dto/MonitorStatusDto.h"
#include <sstream>

namespace nx::cli::serialize {

std::string MonitorStatusJsonSerializer::serialize(const dto::MonitorStatusDto& dto) {
    std::ostringstream json;
    
    json << "{";
    json << "\"engine_id\":\"" << dto.engine_id << "\",";
    json << "\"engine_version\":\"" << dto.engine_version << "\",";
    json << "\"startup_time\":" << dto.startup_time.time_since_epoch().count() << ",";
    json << "\"is_active\":" << (dto.is_active ? "true" : "false") << ",";
    json << "\"current_state\":\"" << dto.current_state << "\",";
    json << "\"active_jobs_count\":" << dto.active_jobs_count << ",";
    json << "\"completed_jobs_count\":" << dto.completed_jobs_count << ",";
    json << "\"failed_jobs_count\":" << dto.failed_jobs_count;
    json << "}";
    
    return json.str();
}

} // namespace nx::cli::serialize