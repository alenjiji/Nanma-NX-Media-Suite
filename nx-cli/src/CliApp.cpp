#include "CliApp.h"
#include "CommandRegistry.h"
#include "CommandId.h"
#include "adapters/MonitorQueryAdapter.h"
#include "dto/MonitorStatusDto.h"
#include "serialize/MonitorStatusJsonSerializer.h"
#include "serialize/MonitorStatusTextSerializer.h"
#include "error/CliError.h"
#include <iostream>

namespace nx::cli {

int CliApp::run(const std::vector<std::string>& args) {
    CommandId command = CommandRegistry::parse(args);
    
    if (command == CommandId::Invalid) {
        return error::CliErrorMapper::to_exit_code(error::CliError::InvalidCommand);
    }
    
    // Check for format flag
    bool has_json = false;
    bool has_text = false;
    for (const auto& arg : args) {
        if (arg == "--json") has_json = true;
        if (arg == "--text") has_text = true;
    }
    
    // Require explicit format flag
    if (!has_json && !has_text) {
        return error::CliErrorMapper::to_exit_code(error::CliError::InvalidCommand);
    }
    
    // Only one format allowed
    if (has_json && has_text) {
        return error::CliErrorMapper::to_exit_code(error::CliError::InvalidCommand);
    }
    
    switch (command) {
        case CommandId::MonitorStatus: {
            nx::monitor::MonitorEngine engine;
            auto status = adapters::MonitorQueryAdapter::query_status(engine);
            dto::MonitorStatusDto dto(status);
            
            if (has_json) {
                std::cout << serialize::MonitorStatusJsonSerializer::serialize(dto);
            } else if (has_text) {
                std::cout << serialize::MonitorStatusTextSerializer::serialize(dto);
            }
            break;
        }
        default:
            break;
    }
    
    return error::CliErrorMapper::to_exit_code(error::CliError::Success);
}

} // namespace nx::cli