#include "CliApp.h"
#include "cli_execution.h"
#include <vector>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    
    // Skip program name (argv[0])
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    
    auto result = nx::cli::execute_command(args);
    
    // Write captured output
    std::cout << result.stdout_text;
    std::cerr << result.stderr_text;
    
    return result.exit_code;
}