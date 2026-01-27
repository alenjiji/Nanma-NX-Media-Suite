#pragma once

#include <vector>
#include <string>

namespace nx::cli {

class CliApp {
public:
    int run(const std::vector<std::string>& args);
};

} // namespace nx::cli