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
    // TODO: Implement status introspection
    output_json("{\"error\": \"Not implemented\", \"subcommand\": \"status\"}");
    return CliResult::ok();
}

CliResult BatchIntrospectionCommand::handle_job(const std::vector<std::string>& args) {
    // TODO: Implement job introspection
    output_json("{\"error\": \"Not implemented\", \"subcommand\": \"job\"}");
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
    // TODO: Implement argument parsing
    return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Argument parsing not implemented");
}

CliResult BatchIntrospectionCommand::parse_job_args(const std::vector<std::string>& args, BatchInspectJobRequest& request) {
    // TODO: Implement argument parsing
    return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Argument parsing not implemented");
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