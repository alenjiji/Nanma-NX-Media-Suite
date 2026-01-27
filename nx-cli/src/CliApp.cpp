#include "CliApp.h"
#include "CommandRegistry.h"
#include "CommandId.h"

namespace nx::cli {

int CliApp::run(const std::vector<std::string>& args) {
    CommandId command = CommandRegistry::parse(args);
    
    if (command == CommandId::Invalid) {
        return 64;
    }
    
    return 0;
}

} // namespace nx::cli