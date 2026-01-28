#pragma once

#include "CommandPresentationMetadata.h"
#include "../../nx-cli/src/CommandId.h"
#include <vector>
#include <optional>
#include <map>

class CommandPresentationRegistry {
public:
    static const std::vector<CommandPresentationMetadata>& getAll();
    static std::optional<CommandGroup> getCommandGroup(nx::cli::CommandId command_id);
    static std::optional<CommandPresentationMetadata> getMetadata(nx::cli::CommandId command_id);
    static std::map<CommandGroup, std::vector<CommandPresentationMetadata>> getGroupedCommands();
    
private:
    static const std::vector<CommandPresentationMetadata> s_metadata;
};