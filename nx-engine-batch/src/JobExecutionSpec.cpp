#include "nx/batch/JobExecutionSpec.h"
#include "../../nx-core/include/identity.h"
#include <sstream>
#include <iomanip>

namespace nx::batch {

JobExecutionSpec JobExecutionSpec::create(
    ComponentType target,
    std::string command,
    std::vector<std::string> arguments,
    RetryPolicy retry_policy,
    FailureStrategy failure_strategy,
    std::vector<JobSpecHash> dependencies) {
    
    auto hash = compute_hash(target, command, arguments, retry_policy, failure_strategy, dependencies);
    
    return JobExecutionSpec(
        std::move(hash),
        target,
        std::move(command),
        std::move(arguments),
        retry_policy,
        failure_strategy,
        std::move(dependencies)
    );
}

JobExecutionSpec::JobExecutionSpec(
    JobSpecHash hash,
    ComponentType target,
    std::string command,
    std::vector<std::string> arguments,
    RetryPolicy retry_policy,
    FailureStrategy failure_strategy,
    std::vector<JobSpecHash> dependencies)
    : hash(std::move(hash))
    , target(target)
    , command(std::move(command))
    , arguments(std::move(arguments))
    , retry_policy(retry_policy)
    , failure_strategy(failure_strategy)
    , dependencies(std::move(dependencies)) {
}

JobSpecHash JobExecutionSpec::compute_hash(
    ComponentType target,
    const std::string& command,
    const std::vector<std::string>& arguments,
    const RetryPolicy& retry_policy,
    FailureStrategy failure_strategy,
    const std::vector<JobSpecHash>& dependencies) {
    
    // Canonical serialization for SHA-256 hashing
    // Fixed field ordering ensures deterministic results across platforms
    std::ostringstream canonical_stream;
    
    // Field 1: Component target (as integer for stability)
    canonical_stream << "target:" << static_cast<int>(target) << ";";
    
    // Field 2: Command string
    canonical_stream << "command:" << command << ";";
    
    // Field 3: Arguments (order preserved for semantic correctness)
    canonical_stream << "arguments:";
    for (const auto& arg : arguments) {
        canonical_stream << arg << ",";
    }
    canonical_stream << ";";
    
    // Field 4: Retry policy
    canonical_stream << "retry_policy:" << retry_policy.max_attempts 
                    << "," << (retry_policy.halt_on_failure ? "1" : "0") << ";";
    
    // Field 5: Failure strategy (as integer for stability)
    canonical_stream << "failure_strategy:" << static_cast<int>(failure_strategy) << ";";
    
    // Field 6: Dependencies (already content-hashed, maintain order)
    canonical_stream << "dependencies:";
    for (const auto& dep : dependencies) {
        canonical_stream << dep.value << ",";
    }
    canonical_stream << ";";
    
    // Compute SHA-256 hash using nx-core implementation
    std::string canonical_content = canonical_stream.str();
    auto hash_bytes = nx::core::Identity::compute_hash(canonical_content);
    
    // Convert to hex string
    std::ostringstream hex_stream;
    hex_stream << std::hex << std::setfill('0');
    for (uint8_t byte : hash_bytes) {
        hex_stream << std::setw(2) << static_cast<unsigned>(byte);
    }
    
    return JobSpecHash{hex_stream.str()};
}

} // namespace nx::batch