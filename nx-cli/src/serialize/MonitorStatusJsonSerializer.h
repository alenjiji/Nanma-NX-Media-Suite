#pragma once

#include <string>

namespace nx::cli::dto {
    struct MonitorStatusDto;
}

namespace nx::cli::serialize {

class MonitorStatusJsonSerializer {
public:
    static std::string serialize(const dto::MonitorStatusDto& dto);
};

} // namespace nx::cli::serialize