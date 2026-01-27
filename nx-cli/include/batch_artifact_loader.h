#pragma once

#include "cli_types.h"
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace nx::cli {

/**
 * Phase 14A Batch Artifact Loaders
 * Read-only access to materialized batch artifacts from completed phases
 */

// Batch Plan Data Structures
struct BatchPlanArtifact {
    std::string batch_id;
    std::string plan_hash;
    size_t job_count;
    std::vector<std::string> job_ids;
    std::map<std::string, std::vector<std::string>> dependencies;  // job_id -> dependency_ids
    std::map<std::string, std::string> job_types;  // job_id -> job_type
    std::map<std::string, size_t> execution_order;  // job_id -> order
};

// Execution State Data Structures
struct JobExecutionState {
    std::string job_id;
    std::string final_state;  // success|failed|skipped
    size_t retry_count;
    std::optional<std::string> failure_classification;
    std::optional<size_t> execution_duration_ms;
};

struct BatchExecutionArtifact {
    std::string batch_id;
    bool execution_complete;
    std::vector<JobExecutionState> job_states;
};

// Policy Resolution Data Structures
struct PolicyResolution {
    std::string job_id;
    std::string policy_type;  // retry|failure|execution
    std::string policy_applied;
    std::string resolved_decision;  // JSON string
    std::string resolution_timestamp;
};

struct BatchPolicyArtifact {
    std::string batch_id;
    std::vector<PolicyResolution> policy_resolutions;
};

// Artifact Metadata Structures
struct ArtifactMetadata {
    std::string artifact_id;
    std::string artifact_type;  // report|validation|hash|timeline|log
    std::string job_id;
    size_t size_bytes;
    std::string created_timestamp;
    std::string content_hash;
};

struct BatchArtifactIndex {
    std::string batch_id;
    std::vector<ArtifactMetadata> artifacts;
};

/**
 * Batch Artifact Loader - Read-only access to materialized artifacts
 */
class BatchArtifactLoader {
public:
    /**
     * Load batch plan artifact by batch ID
     * Returns error if batch not found or plan incomplete
     */
    static CliResult load_batch_plan(const std::string& batch_id, BatchPlanArtifact& plan);
    
    /**
     * Load execution state artifact by batch ID
     * Returns error if batch not found or execution incomplete
     */
    static CliResult load_execution_state(const std::string& batch_id, BatchExecutionArtifact& execution);
    
    /**
     * Load policy resolution artifact by batch ID
     * Returns error if batch not found or policies not resolved
     */
    static CliResult load_policy_resolutions(const std::string& batch_id, BatchPolicyArtifact& policies);
    
    /**
     * Load artifact index by batch ID
     * Returns error if batch not found
     */
    static CliResult load_artifact_index(const std::string& batch_id, BatchArtifactIndex& index);
    
    /**
     * Load specific artifact content by batch ID and artifact ID
     * Returns error if batch or artifact not found
     */
    static CliResult load_artifact_content(const std::string& batch_id, 
                                         const std::string& artifact_id, 
                                         std::string& content);

private:
    // Utility functions for deterministic ordering
    static void sort_jobs_by_execution_order(std::vector<std::string>& job_ids, 
                                            const std::map<std::string, size_t>& execution_order);
    
    static void sort_artifacts_by_id(std::vector<ArtifactMetadata>& artifacts);
    
    // Artifact path resolution (based on existing conventions)
    static std::string get_batch_plan_path(const std::string& batch_id);
    static std::string get_execution_state_path(const std::string& batch_id);
    static std::string get_policy_resolution_path(const std::string& batch_id);
    static std::string get_artifact_index_path(const std::string& batch_id);
    static std::string get_artifact_content_path(const std::string& batch_id, const std::string& artifact_id);
};

} // namespace nx::cli