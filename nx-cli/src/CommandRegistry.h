#pragma once

#include "CommandId.h"
#include <vector>
#include <string>

namespace nx::cli {

class CommandRegistry {
public:
    static CommandId parse(const std::vector<std::string>& args);
};

} // namespace nx::cli