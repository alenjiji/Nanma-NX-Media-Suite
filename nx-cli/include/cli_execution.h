#pragma once

#include <string>
#include <vector>

// Compile-time guard: CommandGroup must never be used in execution paths
#ifdef COMMANDGROUP_H
#error "CommandGroup header detected in CLI execution path - presentation layer leaked into execution"
#endif

namespace nx::cli {

struct CliExecutionResult {
    int exit_code;
    std::string stdout_text;
    std::string stderr_text;
};

CliExecutionResult execute_command(const std::vector<std::string>& args);

} // namespace nx::cli