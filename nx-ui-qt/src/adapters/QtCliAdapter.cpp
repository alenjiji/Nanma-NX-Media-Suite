#include "QtCliAdapter.h"
#include "../commands/CommandDescriptor.h"
#include "../../../nx-cli/include/cli_execution.h"
#include <vector>
#include <string>

// Forward declaration of argv resolution function
std::vector<std::string> resolveCommandArgs(CommandId id);

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

CliResult QtCliAdapter::run(const Command& command)
{
    auto args = resolveCommandArgs(command.getId());
    return run(args);
}