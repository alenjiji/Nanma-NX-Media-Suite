#include "CommandRegistry.h"

// Compile-time guard: CommandGroup must never be used in execution paths
#ifdef COMMANDGROUP_H
#error "CommandGroup header detected in CLI execution path - presentation layer leaked into execution"
#endif

namespace nx::cli {

CommandId CommandRegistry::parse(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return CommandId::Invalid;
    }
    
    const std::string& group = args[0];
    const std::string& command = args[1];
    
    if (group == "monitor") {
        if (command == "status" && args.size() == 2) {
            return CommandId::MonitorStatus;
        }
        if (command == "jobs" && args.size() == 2) {
            return CommandId::MonitorJobs;
        }
        if (command == "job" && args.size() == 3) {
            return CommandId::MonitorJob;
        }
    }
    
    if (group == "batch") {
        if (command == "status" && args.size() == 2) {
            return CommandId::BatchStatus;
        }
        if (command == "session" && args.size() == 3) {
            return CommandId::BatchSession;
        }
        if (command == "jobs" && args.size() == 3) {
            return CommandId::BatchJobs;
        }
    }
    
    return CommandId::Invalid;
}

} // namespace nx::cli