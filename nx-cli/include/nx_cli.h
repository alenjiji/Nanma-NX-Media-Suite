#pragma once

#include "cli_types.h"
#include <vector>
#include <string>

namespace nx::cli {

class NxCli {
public:
    // Main entry point for nx binary
    static int main(int argc, char* argv[]);
    
private:
    // Component routing
    static CliResult route_command(const std::vector<std::string>& args);
    
    // Help system
    static void print_help();
    static void print_component_help(const std::string& component);
    
    // Error handling
    static int handle_result(const CliResult& result);
};

} // namespace nx::cli