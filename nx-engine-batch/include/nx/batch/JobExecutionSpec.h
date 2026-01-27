#pragma once

#include "SessionTypes.h"
#include <vector>
#include <string>
#include <cstdint>

namespace nx::batch {

/**
 * Component target for job execution
 */
enum class ComponentType {
    Convert,    // NX-Convert Pro
    AudioLab,   // NX-AudioLab  
    VideoTrans, // NX-VideoTrans
    MetaFix     // NX-MetaFix
};

/**
 * Retry policy for job execution (declarative only)
 */
struct RetryPolicy {
    uint32_t max_attempts = 1;          // Maximum retry attempts
    bool halt_on_failure = true;        // Halt batch on failure
    
    bool operator==(const RetryPolicy& other) const = default;
};

/**
 * Failure strategy for job execution
 */
enum class FailureStrategy {
    Halt,       // Stop batch execution
    Continue,   // Continue with remaining jobs
    Skip        // Skip dependent jobs only
};

/**
 * Content-derived hash for JobExecutionSpec identity
 */
struct JobSpecHash {
    std::string value;  // SHA-256 hash of spec content
    
    bool operator==(const JobSpecHash& other) const = default;
    bool operator<(const JobSpecHash& other) const { return value < other.value; }
};

/**
 * Immutable job specification for execution intent
 * 
 * IMMUTABILITY GUARANTEE:
 * - All fields are const after construction
 * - No mutation methods provided
 * - Content-hashable for deterministic identity
 * - Two identical specs must produce identical hashes
 * 
 * FORBIDDEN CONTENTS:
 * - Execution state, timestamps, retry counters, progress, runtime IDs
 * 
 * ALLOWED CONTENTS:
 * - Component target, parameters, contracts, policies (declarative only)
 */
struct JobExecutionSpec {
    const JobSpecHash hash;                         // OWNED: Content-derived identity
    const ComponentType target;                     // OWNED: Target component
    const std::string command;                      // OWNED: Command string
    const std::vector<std::string> arguments;       // OWNED: Command arguments
    const RetryPolicy retry_policy;                // OWNED: Retry policy (declarative)
    const FailureStrategy failure_strategy;        // OWNED: Failure handling strategy
    const std::vector<JobSpecHash> dependencies;   // OWNED: Dependencies by spec hash
    
    /**
     * Create JobExecutionSpec with content-derived hash
     * 
     * @param target Component to execute
     * @param command Command string
     * @param arguments Command arguments
     * @param retry_policy Retry policy
     * @param failure_strategy Failure strategy
     * @param dependencies Dependency spec hashes
     * @return Immutable JobExecutionSpec with computed hash
     */
    static JobExecutionSpec create(
        ComponentType target,
        std::string command,
        std::vector<std::string> arguments,
        RetryPolicy retry_policy = {},
        FailureStrategy failure_strategy = FailureStrategy::Halt,
        std::vector<JobSpecHash> dependencies = {}
    );
    
    bool operator==(const JobExecutionSpec& other) const = default;
    
private:
    // Private constructor enforces factory pattern
    JobExecutionSpec(
        JobSpecHash hash,
        ComponentType target,
        std::string command,
        std::vector<std::string> arguments,
        RetryPolicy retry_policy,
        FailureStrategy failure_strategy,
        std::vector<JobSpecHash> dependencies
    );
    
    // Compute content hash from spec fields
    static JobSpecHash compute_hash(
        ComponentType target,
        const std::string& command,
        const std::vector<std::string>& arguments,
        const RetryPolicy& retry_policy,
        FailureStrategy failure_strategy,
        const std::vector<JobSpecHash>& dependencies
    );
};

} // namespace nx::batch