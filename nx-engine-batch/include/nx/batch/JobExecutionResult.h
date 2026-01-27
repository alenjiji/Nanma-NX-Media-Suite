#pragma once

#include <string>

namespace nx::batch {

/**
 * Result descriptor for job execution
 * 
 * PHASE 9 COMPLIANCE:
 * - Contains only execution result information, not identity
 * - JobExecutor operates on JobExecutionSpec (intent), returns execution result
 * - SessionJobId (execution identity) is managed by execution engine, not executor
 */
struct JobExecutionResult {
    bool success;               // OWNED: Execution success/failure
    std::string message;        // OWNED: Execution result message
    std::string result_token;   // OWNED: Opaque execution result identifier
    
    bool operator==(const JobExecutionResult& other) const = default;
};

} // namespace nx::batch