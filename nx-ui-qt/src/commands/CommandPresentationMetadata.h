#pragma once

#include "CommandGroup.h"
#include "../../nx-cli/src/CommandId.h"
#include <string>

struct CommandPresentationMetadata {
    nx::cli::CommandId command_id;
    CommandGroup group;
    std::string short_description;
    std::string long_description;
};