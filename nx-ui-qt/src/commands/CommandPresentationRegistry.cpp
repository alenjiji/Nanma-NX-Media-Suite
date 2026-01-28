#include "CommandPresentationRegistry.h"
#include <algorithm>

const std::vector<CommandPresentationMetadata> CommandPresentationRegistry::s_metadata = {
    {
        nx::cli::CommandId::MonitorStatus,
        CommandGroup::SystemAndInfrastructure,
        "Monitor system status",
        "Display current system monitoring status and engine availability"
    },
    {
        nx::cli::CommandId::MonitorJobs,
        CommandGroup::SystemAndInfrastructure,
        "Monitor active jobs",
        "List all currently active jobs being monitored by the system"
    },
    {
        nx::cli::CommandId::MonitorJob,
        CommandGroup::SystemAndInfrastructure,
        "Monitor specific job",
        "Display detailed monitoring information for a specific job"
    },
    {
        nx::cli::CommandId::BatchStatus,
        CommandGroup::BatchAndAutomation,
        "Batch processing status",
        "Display current batch processing system status and session information"
    },
    {
        nx::cli::CommandId::BatchSession,
        CommandGroup::BatchAndAutomation,
        "Batch session management",
        "Manage batch processing sessions and view session details"
    },
    {
        nx::cli::CommandId::BatchJobs,
        CommandGroup::BatchAndAutomation,
        "Batch job management",
        "List and manage jobs within a specific batch processing session"
    }
};

const std::vector<CommandPresentationMetadata>& CommandPresentationRegistry::getAll() {
    return s_metadata;
}

std::optional<CommandGroup> CommandPresentationRegistry::getCommandGroup(nx::cli::CommandId command_id) {
    auto it = std::find_if(s_metadata.begin(), s_metadata.end(),
        [command_id](const CommandPresentationMetadata& metadata) {
            return metadata.command_id == command_id;
        });
    
    if (it != s_metadata.end()) {
        return it->group;
    }
    return std::nullopt;
}

std::optional<CommandPresentationMetadata> CommandPresentationRegistry::getMetadata(nx::cli::CommandId command_id) {
    auto it = std::find_if(s_metadata.begin(), s_metadata.end(),
        [command_id](const CommandPresentationMetadata& metadata) {
            return metadata.command_id == command_id;
        });
    
    if (it != s_metadata.end()) {
        return *it;
    }
    return std::nullopt;
}

std::map<CommandGroup, std::vector<CommandPresentationMetadata>> CommandPresentationRegistry::getGroupedCommands() {
    std::map<CommandGroup, std::vector<CommandPresentationMetadata>> grouped;
    
    for (const auto& metadata : s_metadata) {
        grouped[metadata.group].push_back(metadata);
    }
    
    return grouped;
}