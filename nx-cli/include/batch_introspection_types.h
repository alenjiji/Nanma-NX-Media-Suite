#pragma once

#include <string>
#include <vector>

namespace nx::cli {

/**
 * Phase 14A Batch Introspection Request Types
 * Read-only access to materialized batch artifacts
 */

struct BatchInspectPlanRequest {
    std::string batch_id;
    
    struct Flags {
        bool json_output = true;  // JSON is default for Phase 14A
        bool jobs_only = false;
        bool dag_only = false;
    } flags;
};

struct BatchInspectJobsRequest {
    std::string batch_id;
    
    struct Flags {
        bool json_output = true;
        std::string filter_type;  // Optional job type filter
        std::string sort = "execution";  // execution|dependency|id
    } flags;
};

struct BatchInspectStatusRequest {
    std::string batch_id;
    
    struct Flags {
        bool json_output = true;
        std::string filter_state;  // Optional state filter: success|failed|skipped
        bool include_retries = false;
    } flags;
};

struct BatchInspectJobRequest {
    std::string batch_id;
    std::string job_id;
    
    struct Flags {
        bool json_output = true;
        bool include_artifacts = false;
        bool include_timeline = false;
    } flags;
};

struct BatchInspectPoliciesRequest {
    std::string batch_id;
    
    struct Flags {
        bool json_output = true;
        std::string job_id;  // Optional job filter
        std::string policy_type;  // Optional: retry|failure|execution
    } flags;
};

struct BatchInspectArtifactsRequest {
    std::string batch_id;
    
    struct Flags {
        bool json_output = true;
        std::string artifact_type;  // Optional: report|validation|hash|timeline|log
        std::string job_id;  // Optional job filter
    } flags;
};

struct BatchInspectArtifactRequest {
    std::string batch_id;
    std::string artifact_id;
    
    struct Flags {
        bool json_output = false;  // Raw format is default for artifact content
        size_t max_size = 0;  // 0 = unlimited
    } flags;
};

} // namespace nx::cli