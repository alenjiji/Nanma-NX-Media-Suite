#include "QtCliAdapter.h"
#include "cli_execution.h"

QtCliAdapter::QtCliAdapter()
{
}

CliResult QtCliAdapter::run(const std::vector<std::string>& args)
{
    auto cli_result = nx::cli::execute_command(args);
    
    CliResult result;
    result.exit_code = cli_result.exit_code;
    result.stdout_text = cli_result.stdout_text;
    result.stderr_text = cli_result.stderr_text;
    
    return result;
}