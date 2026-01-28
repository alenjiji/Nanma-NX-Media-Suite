#pragma once

#include <string>
#include <vector>

namespace nx::cli {

struct CliExecutionResult {
    int exit_code;
    std::string stdout_text;
    std::string stderr_text;
};

CliExecutionResult execute_command(const std::vector<std::string>& args);

} // namespace nx::cli