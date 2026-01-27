#include "CliApp.h"
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    
    // Skip program name (argv[0])
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    
    nx::cli::CliApp app;
    return app.run(args);
}