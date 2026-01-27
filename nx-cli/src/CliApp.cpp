#include "CliApp.h"
#include "CommandRegistry.h"
#include "CommandId.h"
#include "adapters/MonitorQueryAdapter.h"

namespace nx::cli {

int CliApp::run(const std::vector<std::string>& args) {
    CommandId command = CommandRegistry::parse(args);
    
    if (command == CommandId::Invalid) {
        return 64;
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
    
    return 0;
}

} // namespace nx::cli