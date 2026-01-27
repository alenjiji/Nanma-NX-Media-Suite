#pragma once

#include "SessionTypes.h"
#include <string>

namespace nx::batch {

/**
 * Result descriptor for job execution
 * 
 * Contains only the minimal information about execution completion.
 * No success/failure semantics - just execution metadata.
 */
struct JobExecutionResult {
    SessionJobId job_id;        // REFERENCED: Job that was executed
    std::string result_token;   // OPAQUE: Execution result identifier
    
    bool operator==(const JobExecutionResult& other) const = default;
};

} // namespace nx::batch