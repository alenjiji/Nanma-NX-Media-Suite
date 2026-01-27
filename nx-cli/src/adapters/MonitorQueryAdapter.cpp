#include "MonitorQueryAdapter.h"
#include "nx/monitor/MonitorEngine.h"

namespace nx::cli::adapters {

nx::monitor::SystemStatus MonitorQueryAdapter::query_status(const nx::monitor::MonitorEngine& engine) {
    return engine.status();
}

} // namespace nx::cli::adapters