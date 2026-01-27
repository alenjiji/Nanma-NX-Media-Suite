#pragma once

#include "cli_types.h"
#include "batch_introspection_types.h"
#include <vector>
#include <string>

namespace nx::cli {

/**
 * Phase 14A Batch Introspection Command Handler
 * Read-only CLI interface to materialized batch artifacts
 */
class BatchIntrospectionCommand {
public:
    /**
     * Main entry point for batch inspect commands
     * Handles: nx batch inspect <subcommand> [args...]
     */
    static CliResult execute(const std::vector<std::string>& args);

private:
    // Command handlers (skeleton implementations)
    static CliResult handle_plan(const std::vector<std::string>& args);
    static CliResult handle_jobs(const std::vector<std::string>& args);
    static CliResult handle_status(const std::vector<std::string>& args);
    static CliResult handle_job(const std::vector<std::string>& args);
    static CliResult handle_policies(const std::vector<std::string>& args);
    static CliResult handle_artifacts(const std::vector<std::string>& args);
    static CliResult handle_artifact(const std::vector<std::string>& args);
    
    // Argument parsing (to be implemented)
    static CliResult parse_plan_args(const std::vector<std::string>& args, BatchInspectPlanRequest& request);
    static CliResult parse_jobs_args(const std::vector<std::string>& args, BatchInspectJobsRequest& request);
    static CliResult parse_status_args(const std::vector<std::string>& args, BatchInspectStatusRequest& request);
    static CliResult parse_job_args(const std::vector<std::string>& args, BatchInspectJobRequest& request);
    static CliResult parse_policies_args(const std::vector<std::string>& args, BatchInspectPoliciesRequest& request);
    static CliResult parse_artifacts_args(const std::vector<std::string>& args, BatchInspectArtifactsRequest& request);
    static CliResult parse_artifact_args(const std::vector<std::string>& args, BatchInspectArtifactRequest& request);
    
    // JSON output utilities (deterministic serialization)
    static void output_json(const std::string& json_content);
    static std::string escape_json_string(const std::string& input);
};

} // namespace nx::cli