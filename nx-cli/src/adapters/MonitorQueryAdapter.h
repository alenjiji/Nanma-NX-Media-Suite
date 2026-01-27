#pragma once

namespace nx::monitor {
    class MonitorEngine;
    struct SystemStatus;
}

namespace nx::cli::adapters {

class MonitorQueryAdapter {
public:
    static nx::monitor::SystemStatus query_status(const nx::monitor::MonitorEngine& engine);
};

} // namespace nx::cli::adapters