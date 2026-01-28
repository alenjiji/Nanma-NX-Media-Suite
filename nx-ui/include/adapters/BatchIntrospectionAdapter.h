// PHASE 15 INVARIANT:
// This adapter is a stateless CLI bridge.
// It adds no defaults, no retries, no inference.
// All parameters map 1:1 to CLI arguments.
// If a value is missing, this is a caller error.

#pragma once

#include <string>

namespace nx::ui::adapters {

struct BatchInspectPlanDto {};
struct BatchInspectJobsDto {};
struct BatchInspectStatusDto {};
struct BatchInspectJobDto {};
struct BatchInspectPoliciesDto {};
struct BatchInspectArtifactsDto {};
struct BatchInspectArtifactDto {};

template <typename Dto>
struct CliTypedResult {
    int exit_code;
    std::string raw_json;
};

struct BatchInspectPlanArgs {
    std::string batch_id;
    std::string format;
    bool jobs_only;
    bool dag_only;
};

struct BatchInspectJobsArgs {
    std::string batch_id;
    std::string format;
    std::string filter_type;
    std::string sort;
};

struct BatchInspectStatusArgs {
    std::string batch_id;
    std::string format;
    std::string filter_state;
    bool include_retries; // Maps directly to CLI flag --include-retries (no UI inference)
};

struct BatchInspectJobArgs {
    std::string batch_id;
    std::string job_id;
    std::string format;
    bool include_artifacts;
    bool include_timeline; // Maps directly to CLI flag --include-timeline (no UI inference)
};

struct BatchInspectPoliciesArgs {
    std::string batch_id;
    std::string format;
    std::string job_id;
    std::string policy_type;
};

struct BatchInspectArtifactsArgs {
    std::string batch_id;
    std::string format;
    std::string artifact_type;
    std::string job_id;
};

struct BatchInspectArtifactArgs {
    std::string batch_id;
    std::string artifact_id;
    std::string format;
    int max_size;
};

class BatchIntrospectionAdapter {
public:
    static CliTypedResult<BatchInspectPlanDto> plan(const BatchInspectPlanArgs& args);
    static CliTypedResult<BatchInspectJobsDto> jobs(const BatchInspectJobsArgs& args);
    static CliTypedResult<BatchInspectStatusDto> status(const BatchInspectStatusArgs& args);
    static CliTypedResult<BatchInspectJobDto> job(const BatchInspectJobArgs& args);
    static CliTypedResult<BatchInspectPoliciesDto> policies(const BatchInspectPoliciesArgs& args);
    static CliTypedResult<BatchInspectArtifactsDto> artifacts(const BatchInspectArtifactsArgs& args);
    static CliTypedResult<BatchInspectArtifactDto> artifact(const BatchInspectArtifactArgs& args);
};

} // namespace nx::ui::adapters