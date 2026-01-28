#pragma once

#include <vector>
#include <string>

enum class CommandId {
    Version,
    Help
};

std::vector<std::string> getCommandArgs(CommandId id);