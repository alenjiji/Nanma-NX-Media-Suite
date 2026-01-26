#pragma once

#include "nx_batchflow_jobid.h"
#include "nx_batchflow_retry_policy.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

namespace nx::batchflow {

/// PresetVersion represents semantic versioning for preset compatibility
/// Immutable version identifier for preset schema evolution
struct PresetVersion {
    uint32_t major;     // Breaking changes
    uint32_t minor;     // Backward-compatible additions
    uint32_t patch;     // Bug fixes and clarifications
    
    PresetVersion(uint32_t maj, uint32_t min, uint32_t pat)
        : major(maj), minor(min), patch(pat) {}
    
    /// Current preset schema version
    static PresetVersion current() { return PresetVersion(1, 0, 0); }
    
    /// Check version compatibility (same major version)
    bool is_compatible_with(const PresetVersion& other) const {
        return major == other.major;
    }
    
    /// String representation for serialization
    std::string to_string() const {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
    
    /// Parse version from string
    static PresetVersion from_string(const std::string& version_str);
    
    /// Comparison operators for versioning
    bool operator==(const PresetVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator<(const PresetVersion& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
};

/// PresetJobDefinition represents a single job in the preset
/// Pure data structure - no logic, no conditionals, no expressions
struct PresetJobDefinition {
    std::string job_name;               // Human-readable job identifier
    std::string engine_identifier;     // Target engine name (opaque string)
    std::string api_operation;         // Engine API operation (opaque string)
    std::string parameters_blob;       // Canonicalized parameters (opaque string)
    std::vector<std::string> input_artifacts;   // Input artifact identifiers
    std::vector<std::string> output_artifacts;  // Output artifact identifiers
    
    /// Default constructor for container usage
    PresetJobDefinition() = default;
    
    /// Create job definition with all required fields
    PresetJobDefinition(std::string name,
                       std::string engine_id,
                       std::string operation,
                       std::string params,
                       std::vector<std::string> inputs,
                       std::vector<std::string> outputs)
        : job_name(std::move(name))
        , engine_identifier(std::move(engine_id))
        , api_operation(std::move(operation))
        , parameters_blob(std::move(params))
        , input_artifacts(std::move(inputs))
        , output_artifacts(std::move(outputs)) {}
    
    /// Equality for diffing and validation
    bool operator==(const PresetJobDefinition& other) const {
        return job_name == other.job_name &&
               engine_identifier == other.engine_identifier &&
               api_operation == other.api_operation &&
               parameters_blob == other.parameters_blob &&
               input_artifacts == other.input_artifacts &&
               output_artifacts == other.output_artifacts;
    }
};

/// PresetDependency represents a dependency relationship between jobs
/// Pure declarative data - no conditional logic
struct PresetDependency {
    std::string from_job;   // Job that must complete first
    std::string to_job;     // Job that depends on from_job
    
    PresetDependency(std::string from, std::string to)
        : from_job(std::move(from)), to_job(std::move(to)) {}
    
    /// Equality for diffing and validation
    bool operator==(const PresetDependency& other) const {
        return from_job == other.from_job && to_job == other.to_job;
    }
    
    /// Ordering for deterministic serialization
    bool operator<(const PresetDependency& other) const {
        if (from_job != other.from_job) return from_job < other.from_job;
        return to_job < other.to_job;
    }
};

/// PresetRetryPolicy represents retry configuration for jobs
/// Pure data structure mapping job names to retry policies
struct PresetRetryPolicy {
    std::string job_name;       // Which job this policy applies to
    uint32_t max_attempts;      // Maximum total attempts
    uint32_t retry_delay_ticks; // Fixed delay in logical ticks
    std::set<std::string> retry_on_states; // States that trigger retry (as strings)
    
    /// Default constructor for container usage
    PresetRetryPolicy() : max_attempts(1), retry_delay_ticks(0) {}
    
    PresetRetryPolicy(std::string name, uint32_t max_att, uint32_t delay, std::set<std::string> states)
        : job_name(std::move(name))
        , max_attempts(max_att)
        , retry_delay_ticks(delay)
        , retry_on_states(std::move(states)) {}
    
    /// Convert to runtime RetryPolicy
    RetryPolicy to_runtime_policy() const;
    
    /// Equality for diffing and validation
    bool operator==(const PresetRetryPolicy& other) const {
        return job_name == other.job_name &&
               max_attempts == other.max_attempts &&
               retry_delay_ticks == other.retry_delay_ticks &&
               retry_on_states == other.retry_on_states;
    }
};

/// BatchFlowPreset represents complete workflow definition
/// Pure data structure - immutable at runtime, no logic, no conditionals
class BatchFlowPreset {
public:
    /// Create preset with version and metadata
    BatchFlowPreset(PresetVersion version, std::string name, std::string description)
        : version_(version)
        , preset_name_(std::move(name))
        , description_(std::move(description)) {}
    
    /// Get preset version
    const PresetVersion& version() const { return version_; }
    
    /// Get preset name
    const std::string& name() const { return preset_name_; }
    
    /// Get preset description
    const std::string& description() const { return description_; }
    
    /// Add job definition to preset
    void add_job(PresetJobDefinition job);
    
    /// Add dependency to preset
    void add_dependency(PresetDependency dependency);
    
    /// Add retry policy to preset
    void add_retry_policy(PresetRetryPolicy policy);
    
    /// Get all job definitions (ordered by name for determinism)
    const std::map<std::string, PresetJobDefinition>& jobs() const { return jobs_; }
    
    /// Get all dependencies (ordered for determinism)
    const std::set<PresetDependency>& dependencies() const { return dependencies_; }
    
    /// Get all retry policies (ordered by job name for determinism)
    const std::map<std::string, PresetRetryPolicy>& retry_policies() const { return retry_policies_; }
    
    /// Serialize preset to JSON string
    std::string to_json() const;
    
    /// Deserialize preset from JSON string
    static BatchFlowPreset from_json(const std::string& json_str);
    
    /// Validate preset structure and consistency
    std::vector<std::string> validate() const;
    
    /// Check if preset is valid (no validation errors)
    bool is_valid() const { return validate().empty(); }
    
    /// Generate diff between two presets
    std::vector<std::string> diff(const BatchFlowPreset& other) const;
    
    /// Equality for comparison
    bool operator==(const BatchFlowPreset& other) const;

private:
    PresetVersion version_;                                     // Schema version
    std::string preset_name_;                                   // Human-readable name
    std::string description_;                                   // Preset description
    std::map<std::string, PresetJobDefinition> jobs_;         // Job definitions (ordered)
    std::set<PresetDependency> dependencies_;                  // Dependencies (ordered)
    std::map<std::string, PresetRetryPolicy> retry_policies_; // Retry policies (ordered)
};

/// PresetValidator provides validation logic for preset consistency
/// Pure validation functions - no state, no side effects
class PresetValidator {
public:
    /// Validate job definitions for consistency
    static std::vector<std::string> validate_jobs(const std::map<std::string, PresetJobDefinition>& jobs);
    
    /// Validate dependencies for cycles and missing jobs
    static std::vector<std::string> validate_dependencies(
        const std::map<std::string, PresetJobDefinition>& jobs,
        const std::set<PresetDependency>& dependencies);
    
    /// Validate retry policies reference existing jobs
    static std::vector<std::string> validate_retry_policies(
        const std::map<std::string, PresetJobDefinition>& jobs,
        const std::map<std::string, PresetRetryPolicy>& policies);
    
    /// Validate preset version compatibility
    static std::vector<std::string> validate_version(const PresetVersion& version);

private:
    /// Check for dependency cycles using DFS
    static bool has_dependency_cycle(const std::set<PresetDependency>& dependencies);
    
    /// Validate individual job definition
    static std::vector<std::string> validate_job(const PresetJobDefinition& job);
};

/// Implementation of key methods
inline void BatchFlowPreset::add_job(PresetJobDefinition job) {
    std::string job_name = job.job_name;
    jobs_[job_name] = std::move(job);
}

inline void BatchFlowPreset::add_dependency(PresetDependency dependency) {
    dependencies_.insert(std::move(dependency));
}

inline void BatchFlowPreset::add_retry_policy(PresetRetryPolicy policy) {
    std::string job_name = policy.job_name;
    retry_policies_[job_name] = std::move(policy);
}

inline std::vector<std::string> BatchFlowPreset::validate() const {
    std::vector<std::string> errors;
    
    // Validate version
    auto version_errors = PresetValidator::validate_version(version_);
    errors.insert(errors.end(), version_errors.begin(), version_errors.end());
    
    // Validate jobs
    auto job_errors = PresetValidator::validate_jobs(jobs_);
    errors.insert(errors.end(), job_errors.begin(), job_errors.end());
    
    // Validate dependencies
    auto dep_errors = PresetValidator::validate_dependencies(jobs_, dependencies_);
    errors.insert(errors.end(), dep_errors.begin(), dep_errors.end());
    
    // Validate retry policies
    auto retry_errors = PresetValidator::validate_retry_policies(jobs_, retry_policies_);
    errors.insert(errors.end(), retry_errors.begin(), retry_errors.end());
    
    return errors;
}

inline bool BatchFlowPreset::operator==(const BatchFlowPreset& other) const {
    return version_ == other.version_ &&
           preset_name_ == other.preset_name_ &&
           description_ == other.description_ &&
           jobs_ == other.jobs_ &&
           dependencies_ == other.dependencies_ &&
           retry_policies_ == other.retry_policies_;
}

inline RetryPolicy PresetRetryPolicy::to_runtime_policy() const {
    std::set<RetryableState> runtime_states;
    for (const auto& state_str : retry_on_states) {
        if (state_str == "Failed") {
            runtime_states.insert(RetryableState::Failed);
        }
    }
    
    return RetryPolicy(max_attempts, runtime_states, retry_delay_ticks);
}

} // namespace nx::batchflow