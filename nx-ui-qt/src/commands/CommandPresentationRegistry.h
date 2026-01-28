#pragma once

#include "CommandPresentationMetadata.h"
#include <vector>

class CommandPresentationRegistry {
public:
    static const std::vector<CommandPresentationMetadata>& getAll();
    
private:
    static const std::vector<CommandPresentationMetadata> s_metadata;
};