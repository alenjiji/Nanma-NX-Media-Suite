#pragma once

#include "SessionTypes.h"
#include <vector>
#include <string>

namespace nx::batch {

/**
 * Immutable job specification for execution
 * 
 * Contains all information needed to execute a single job in isolation.
 * This is a value type that cannot be mutated after construction.
 */
struct JobExecutionSpec {
    SessionJobId job_id;                    // REFERENCED: Job identity from session
    std::string command;                    // COPIED: Command string from session
    std::vector<std::string> arguments;     // COPIED: Arguments from session
    
    bool operator==(const JobExecutionSpec& other) const = default;
};

} // namespace nx::batch