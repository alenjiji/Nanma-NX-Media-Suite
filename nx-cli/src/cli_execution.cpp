#include "cli_execution.h"
#include "CliApp.h"
#include <sstream>
#include <iostream>

namespace nx::cli {

CliExecutionResult execute_command(const std::vector<std::string>& args) {
    // Capture stdout/stderr
    std::ostringstream captured_stdout;
    std::ostringstream captured_stderr;
    
    // Save original streams
    std::streambuf* orig_cout = std::cout.rdbuf();
    std::streambuf* orig_cerr = std::cerr.rdbuf();
    
    // Redirect streams
    std::cout.rdbuf(captured_stdout.rdbuf());
    std::cerr.rdbuf(captured_stderr.rdbuf());
    
    // Execute command
    CliApp app;
    int exit_code = app.run(args);
    
    // Restore streams
    std::cout.rdbuf(orig_cout);
    std::cerr.rdbuf(orig_cerr);
    
    CliExecutionResult result;
    result.exit_code = exit_code;
    result.stdout_text = captured_stdout.str();
    result.stderr_text = captured_stderr.str();
    
    return result;
}

} // namespace nx::cli