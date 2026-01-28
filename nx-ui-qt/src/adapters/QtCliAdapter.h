#pragma once

#include <string>
#include <vector>

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
};