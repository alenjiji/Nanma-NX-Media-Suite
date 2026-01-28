#pragma once

#include <string>
#include <vector>
#include "../../nx-cli/src/CommandId.h"

class Command;

struct CliResult {
    int exit_code;
    std::string stdout_text;
    std::string stderr_text;
};

class QtCliAdapter
{
public:
    QtCliAdapter();
    
    CliResult run(const std::vector<std::string>& args);
    CliResult run(const Command& command);
    CliResult runCommand(nx::cli::CommandId command_id);
    
private:
    std::vector<std::string> getArgsForCommand(nx::cli::CommandId command_id);
};