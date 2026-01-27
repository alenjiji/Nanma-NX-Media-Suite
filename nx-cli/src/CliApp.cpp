#include "CliApp.h"
#include "CommandRegistry.h"
#include "CommandId.h"
#include "adapters/MonitorQueryAdapter.h"
#include "error/CliError.h"

namespace nx::cli {

int CliApp::run(const std::vector<std::string>& args) {
    CommandId command = CommandRegistry::parse(args);
    
    if (command == CommandId::Invalid) {
        return error::CliErrorMapper::to_exit_code(error::CliError::InvalidCommand);
    }
    
    switch (command) {
        case CommandId::MonitorStatus: {
            nx::monitor::MonitorEngine engine;
            static_cast<void>(adapters::MonitorQueryAdapter::query_status(engine));
            break;
        }
        default:
            break;
    }
    
    return error::CliErrorMapper::to_exit_code(error::CliError::Success);
}

} // namespace nx::cli