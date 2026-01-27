#include "batch_introspection_command.h"
#include "batch_artifact_loader.h"
#include <iostream>
#include <map>
#include <algorithm>

namespace nx::cli {

CliResult BatchIntrospectionCommand::execute(const std::vector<std::string>& args) {
    if (args.empty() || args[0] == "--help" || args[0] == "-h") {
        std::cout << "nx batch inspect - Read-only batch introspection\\n\\n";
        std::cout << "Subcommands:\\n";
        std::cout << "  plan        Display batch plan structure and DAG\\n";
        std::cout << "  jobs        List all jobs in batch with metadata\\n";
        std::cout << "  status      Show materialized execution state for all jobs\\n";
        std::cout << "  job         Detailed view of single job execution\\n";
        std::cout << "  policies    Show resolved policy decisions for batch\\n";
        std::cout << "  artifacts   List all artifacts produced by batch execution\\n";
        std::cout << "  artifact    Display specific artifact content\\n\\n";
        std::cout << "Use 'nx batch inspect <subcommand> --help' for subcommand-specific help\\n";
        return CliResult::ok();
    }
    
    const std::string& subcommand = args[0];
    std::vector<std::string> subcommand_args(args.begin() + 1, args.end());
    
    // Route to appropriate handler
    if (subcommand == "plan") {
        return handle_plan(subcommand_args);
    } else if (subcommand == "jobs") {
        return handle_jobs(subcommand_args);
    } else if (subcommand == "status") {
        return handle_status(subcommand_args);
    } else if (subcommand == "job") {
        return handle_job(subcommand_args);
    } else if (subcommand == "policies") {
        return handle_policies(subcommand_args);
    } else if (subcommand == "artifacts") {
        return handle_artifacts(subcommand_args);
    } else if (subcommand == "artifact") {
        return handle_artifact(subcommand_args);
    } else {
        return CliResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Unknown batch inspect subcommand: " + subcommand + 
            ". Available: plan, jobs, status, job, policies, artifacts, artifact"
        );
    }
}

CliResult BatchIntrospectionCommand::handle_plan(const std::vector<std::string>& args) {
    BatchInspectPlanRequest request;
    auto parse_result = parse_plan_args(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    // Load batch plan artifact
    BatchPlanArtifact plan;
    auto load_result = BatchArtifactLoader::load_batch_plan(request.batch_id, plan);
    if (!load_result.success) {
        return load_result;
    }
    
    // Generate JSON output according to contract schema
    std::string json = "{\n";
    json += "  \"batch_id\": \"" + escape_json_string(plan.batch_id) + "\",\n";
    json += "  \"plan_hash\": \"" + escape_json_string(plan.plan_hash) + "\",\n";
    json += "  \"job_count\": " + std::to_string(plan.job_count) + ",\n";
    
    // Jobs array (unless --dag-only)
    if (!request.flags.dag_only) {
        json += "  \"jobs\": [\n";
        for (size_t i = 0; i < plan.job_ids.size(); ++i) {
            const std::string& job_id = plan.job_ids[i];
            json += "    {\n";
            json += "      \"job_id\": \"" + escape_json_string(job_id) + "\",\n";
            json += "      \"job_type\": \"" + escape_json_string(plan.job_types.at(job_id)) + "\",\n";
            
            // Dependencies array
            json += "      \"dependencies\": [";
            const auto& deps = plan.dependencies.at(job_id);
            for (size_t j = 0; j < deps.size(); ++j) {
                json += "\"" + escape_json_string(deps[j]) + "\"";
                if (j < deps.size() - 1) json += ", ";
            }
            json += "],\n";
            
            json += "      \"execution_order\": " + std::to_string(plan.execution_order.at(job_id)) + "\n";
            json += "    }";
            if (i < plan.job_ids.size() - 1) json += ",";
            json += "\n";
        }
        json += "  ]";
        if (!request.flags.jobs_only) json += ",";
        json += "\n";
    }
    
    // DAG (unless --jobs-only)
    if (!request.flags.jobs_only) {
        json += "  \"dag\": {\n";
        
        // Nodes (sorted by job ID for determinism)
        std::vector<std::string> sorted_nodes = plan.job_ids;
        std::sort(sorted_nodes.begin(), sorted_nodes.end());
        
        json += "    \"nodes\": [";
        for (size_t i = 0; i < sorted_nodes.size(); ++i) {
            json += "\"" + escape_json_string(sorted_nodes[i]) + "\"";
            if (i < sorted_nodes.size() - 1) json += ", ";
        }
        json += "],\n";
        
        // Edges (sorted lexicographically)
        json += "    \"edges\": [";
        std::vector<std::pair<std::string, std::string>> edges;
        for (const auto& [job_id, deps] : plan.dependencies) {
            for (const std::string& dep : deps) {
                edges.emplace_back(dep, job_id);
            }
        }
        std::sort(edges.begin(), edges.end());
        
        for (size_t i = 0; i < edges.size(); ++i) {
            json += "[\"" + escape_json_string(edges[i].first) + "\", \"" + escape_json_string(edges[i].second) + "\"]";
            if (i < edges.size() - 1) json += ", ";
        }
        json += "]\n";
        
        json += "  }\n";
    }
    
    json += "}";
    
    output_json(json);
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::handle_jobs(const std::vector<std::string>& args) {
    BatchInspectJobsRequest request;
    auto parse_result = parse_jobs_args(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    // Load batch plan artifact
    BatchPlanArtifact plan;
    auto load_result = BatchArtifactLoader::load_batch_plan(request.batch_id, plan);
    if (!load_result.success) {
        return load_result;
    }
    
    // Prepare job list with metadata
    std::vector<std::string> job_list = plan.job_ids;
    
    // Apply filtering if specified
    if (!request.flags.filter_type.empty()) {
        std::vector<std::string> filtered_jobs;
        for (const std::string& job_id : job_list) {
            if (plan.job_types.at(job_id) == request.flags.filter_type) {
                filtered_jobs.push_back(job_id);
            }
        }
        job_list = filtered_jobs;
    }
    
    // Apply sorting
    if (request.flags.sort == "id") {
        std::sort(job_list.begin(), job_list.end());
    } else if (request.flags.sort == "dependency") {
        std::sort(job_list.begin(), job_list.end(), [&plan](const std::string& a, const std::string& b) {
            size_t deps_a = plan.dependencies.at(a).size();
            size_t deps_b = plan.dependencies.at(b).size();
            if (deps_a != deps_b) return deps_a < deps_b;
            return a < b; // Stable sort by ID
        });
    } else { // execution (default)
        BatchArtifactLoader::sort_jobs_by_execution_order(job_list, plan.execution_order);
    }
    
    // Generate JSON output according to contract schema
    std::string json = "{\n";
    json += "  \"batch_id\": \"" + escape_json_string(plan.batch_id) + "\",\n";
    json += "  \"jobs\": [\n";
    
    for (size_t i = 0; i < job_list.size(); ++i) {
        const std::string& job_id = job_list[i];
        
        // Calculate dependent count (jobs that depend on this job)
        size_t dependent_count = 0;
        for (const auto& [other_job, deps] : plan.dependencies) {
            if (std::find(deps.begin(), deps.end(), job_id) != deps.end()) {
                dependent_count++;
            }
        }
        
        json += "    {\n";
        json += "      \"job_id\": \"" + escape_json_string(job_id) + "\",\n";
        json += "      \"job_type\": \"" + escape_json_string(plan.job_types.at(job_id)) + "\",\n";
        json += "      \"execution_order\": " + std::to_string(plan.execution_order.at(job_id)) + ",\n";
        json += "      \"dependency_count\": " + std::to_string(plan.dependencies.at(job_id).size()) + ",\n";
        json += "      \"dependent_count\": " + std::to_string(dependent_count) + "\n";
        json += "    }";
        if (i < job_list.size() - 1) json += ",";
        json += "\n";
    }
    
    json += "  ]\n";
    json += "}";
    
    output_json(json);
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::handle_status(const std::vector<std::string>& args) {
    BatchInspectStatusRequest request;
    auto parse_result = parse_status_args(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    // Load execution state artifact
    BatchExecutionArtifact execution;
    auto load_result = BatchArtifactLoader::load_execution_state(request.batch_id, execution);
    if (!load_result.success) {
        return load_result;
    }
    
    // Check if execution is complete
    if (!execution.execution_complete) {
        return CliResult::error(CliErrorCode::ERROR_EXECUTION_INCOMPLETE, 
                               "Batch execution not complete for ID: " + request.batch_id);
    }
    
    // Apply state filtering if specified
    std::vector<JobExecutionState> filtered_states = execution.job_states;
    if (!request.flags.filter_state.empty()) {
        std::vector<JobExecutionState> temp_states;
        for (const auto& state : filtered_states) {
            if (state.final_state == request.flags.filter_state) {
                temp_states.push_back(state);
            }
        }
        filtered_states = temp_states;
    }
    
    // Sort by execution order (assuming job_id contains order info), then by job_id
    std::sort(filtered_states.begin(), filtered_states.end(), [](const JobExecutionState& a, const JobExecutionState& b) {
        return a.job_id < b.job_id; // Deterministic sort by job_id
    });
    
    // Generate JSON output according to contract schema
    std::string json = "{\n";
    json += "  \"batch_id\": \"" + escape_json_string(execution.batch_id) + "\",\n";
    json += "  \"execution_complete\": " + (execution.execution_complete ? "true" : "false") + ",\n";
    json += "  \"job_states\": [\n";
    
    for (size_t i = 0; i < filtered_states.size(); ++i) {
        const auto& state = filtered_states[i];
        
        json += "    {\n";
        json += "      \"job_id\": \"" + escape_json_string(state.job_id) + "\",\n";
        json += "      \"final_state\": \"" + escape_json_string(state.final_state) + "\",\n";
        
        if (request.flags.include_retries) {
            json += "      \"retry_count\": " + std::to_string(state.retry_count) + ",\n";
        }
        
        if (state.failure_classification.has_value()) {
            json += "      \"failure_classification\": \"" + escape_json_string(state.failure_classification.value()) + "\",\n";
        } else {
            json += "      \"failure_classification\": null,\n";
        }
        
        if (state.execution_duration_ms.has_value()) {
            json += "      \"execution_duration_ms\": " + std::to_string(state.execution_duration_ms.value()) + "\n";
        } else {
            json += "      \"execution_duration_ms\": null\n";
        }
        
        json += "    }";
        if (i < filtered_states.size() - 1) json += ",";
        json += "\n";
    }
    
    json += "  ]\n";
    json += "}";
    
    output_json(json);
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::handle_job(const std::vector<std::string>& args) {
    BatchInspectJobRequest request;
    auto parse_result = parse_job_args(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    // Load execution state artifact
    BatchExecutionArtifact execution;
    auto load_result = BatchArtifactLoader::load_execution_state(request.batch_id, execution);
    if (!load_result.success) {
        return load_result;
    }
    
    // Check if execution is complete
    if (!execution.execution_complete) {
        return CliResult::error(CliErrorCode::ERROR_EXECUTION_INCOMPLETE, 
                               "Batch execution not complete for ID: " + request.batch_id);
    }
    
    // Find the specific job
    JobExecutionState* job_state = nullptr;
    for (auto& state : execution.job_states) {
        if (state.job_id == request.job_id) {
            job_state = &state;
            break;
        }
    }
    
    if (!job_state) {
        return CliResult::error(CliErrorCode::ERROR_JOB_NOT_FOUND, 
                               "Job not found: " + request.job_id + " in batch: " + request.batch_id);
    }
    
    // Generate JSON output according to contract schema
    std::string json = "{\n";
    json += "  \"batch_id\": \"" + escape_json_string(execution.batch_id) + "\",\n";
    json += "  \"job_id\": \"" + escape_json_string(job_state->job_id) + "\",\n";
    json += "  \"job_type\": \"" + escape_json_string(job_state->job_type) + "\",\n";
    json += "  \"final_state\": \"" + escape_json_string(job_state->final_state) + "\",\n";
    json += "  \"retry_count\": " + std::to_string(job_state->retry_count) + ",\n";
    
    if (job_state->failure_classification.has_value()) {
        json += "  \"failure_classification\": \"" + escape_json_string(job_state->failure_classification.value()) + "\",\n";
    } else {
        json += "  \"failure_classification\": null,\n";
    }
    
    // Timeline (only if requested)
    if (request.flags.include_timeline) {
        json += "  \"execution_timeline\": [],\n"; // Empty for now - no timeline data available
    }
    
    // Artifacts (only if requested)
    if (request.flags.include_artifacts) {
        json += "  \"artifacts\": []\n"; // Empty for now - no artifact data available
    } else {
        // Remove trailing comma if no artifacts/timeline
        if (!request.flags.include_timeline) {
            json.pop_back(); json.pop_back(); // Remove ",\n"
            json += "\n";
        }
    }
    
    json += "}";
    
    output_json(json);
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::handle_policies(const std::vector<std::string>& args) {
    // TODO: Implement policies introspection
    output_json("{\"error\": \"Not implemented\", \"subcommand\": \"policies\"}");
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::handle_artifacts(const std::vector<std::string>& args) {
    // TODO: Implement artifacts introspection
    output_json("{\"error\": \"Not implemented\", \"subcommand\": \"artifacts\"}");
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::handle_artifact(const std::vector<std::string>& args) {
    // TODO: Implement artifact content introspection
    output_json("{\"error\": \"Not implemented\", \"subcommand\": \"artifact\"}");
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::parse_plan_args(const std::vector<std::string>& args, BatchInspectPlanRequest& request) {
    if (args.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required batch_id");
    }
    
    request.batch_id = args[0];
    
    // Parse flags
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--jobs-only") {
            request.flags.jobs_only = true;
        } else if (arg == "--dag-only") {
            request.flags.dag_only = true;
        } else if (arg == "--format") {
            if (i + 1 >= args.size()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--format requires value");
            }
            const std::string& format = args[++i];
            if (format != "json" && format != "human") {
                return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid format: " + format + ". Must be json|human");
            }
            // JSON is default, no need to store format preference for now
        } else {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown flag: " + arg);
        }
    }
    
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::parse_jobs_args(const std::vector<std::string>& args, BatchInspectJobsRequest& request) {
    if (args.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required batch_id");
    }
    
    request.batch_id = args[0];
    
    // Parse flags
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--filter-type") {
            if (i + 1 >= args.size()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--filter-type requires value");
            }
            request.flags.filter_type = args[++i];
        } else if (arg == "--sort") {
            if (i + 1 >= args.size()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--sort requires value");
            }
            const std::string& sort = args[++i];
            if (sort != "execution" && sort != "dependency" && sort != "id") {
                return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid sort: " + sort + ". Must be execution|dependency|id");
            }
            request.flags.sort = sort;
        } else if (arg == "--format") {
            if (i + 1 >= args.size()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--format requires value");
            }
            const std::string& format = args[++i];
            if (format != "json" && format != "human") {
                return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid format: " + format + ". Must be json|human");
            }
            // JSON is default, no need to store format preference for now
        } else {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown flag: " + arg);
        }
    }
    
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::parse_status_args(const std::vector<std::string>& args, BatchInspectStatusRequest& request) {
    if (args.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required batch_id");
    }
    
    request.batch_id = args[0];
    
    // Parse flags
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--filter-state") {
            if (i + 1 >= args.size()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--filter-state requires value");
            }
            const std::string& state = args[++i];
            if (state != "success" && state != "failed" && state != "skipped") {
                return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid state: " + state + ". Must be success|failed|skipped");
            }
            request.flags.filter_state = state;
        } else if (arg == "--include-retries") {
            request.flags.include_retries = true;
        } else if (arg == "--format") {
            if (i + 1 >= args.size()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--format requires value");
            }
            const std::string& format = args[++i];
            if (format != "json" && format != "human") {
                return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid format: " + format + ". Must be json|human");
            }
            // JSON is default, no need to store format preference for now
        } else {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown flag: " + arg);
        }
    }
    
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::parse_job_args(const std::vector<std::string>& args, BatchInspectJobRequest& request) {
    if (args.size() < 2) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required batch_id and job_id");
    }
    
    request.batch_id = args[0];
    request.job_id = args[1];
    
    // Parse flags
    for (size_t i = 2; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--include-artifacts") {
            request.flags.include_artifacts = true;
        } else if (arg == "--include-timeline") {
            request.flags.include_timeline = true;
        } else if (arg == "--format") {
            if (i + 1 >= args.size()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--format requires value");
            }
            const std::string& format = args[++i];
            if (format != "json" && format != "human") {
                return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid format: " + format + ". Must be json|human");
            }
            // JSON is default, no need to store format preference for now
        } else {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown flag: " + arg);
        }
    }
    
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::parse_policies_args(const std::vector<std::string>& args, BatchInspectPoliciesRequest& request) {
    // TODO: Implement argument parsing
    return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Argument parsing not implemented");
}

CliResult BatchIntrospectionCommand::parse_artifacts_args(const std::vector<std::string>& args, BatchInspectArtifactsRequest& request) {
    // TODO: Implement argument parsing
    return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Argument parsing not implemented");
}

CliResult BatchIntrospectionCommand::parse_artifact_args(const std::vector<std::string>& args, BatchInspectArtifactRequest& request) {
    // TODO: Implement argument parsing
    return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Argument parsing not implemented");
}

void BatchIntrospectionCommand::output_json(const std::string& json_content) {
    std::cout << json_content << std::endl;
}

std::string BatchIntrospectionCommand::escape_json_string(const std::string& input) {
    std::string result;
    result.reserve(input.length() + 10);  // Reserve extra space for escapes
    
    for (char c : input) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:   result += c; break;
        }
    }
    
    return result;
}

} // namespace nx::cli