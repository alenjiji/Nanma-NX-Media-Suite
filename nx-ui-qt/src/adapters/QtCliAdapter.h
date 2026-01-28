#pragma once

#include <string>
#include <vector>

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
};