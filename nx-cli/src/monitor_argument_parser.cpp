#include "monitor_argument_parser.h"
#include <algorithm>
#include <set>

namespace nx::cli {

MonitorParseResult MonitorArgumentParser::parse_status_args(const std::vector<std::string>& args, MonitorStatusRequest& request) {
    std::vector<std::string> allowed_flags = {"--json"};
    
    auto validation_result = validate_no_unknown_flags(args, allowed_flags);
    if (!validation_result.success) {
        return validation_result;
    }
    
    auto duplicate_result = validate_no_duplicates(args);
    if (!duplicate_result.success) {
        return duplicate_result;
    }
    
    request.flags.json_output = has_flag(args, "--json");
    
    return MonitorParseResult::ok();
}

MonitorParseResult MonitorArgumentParser::parse_jobs_args(const std::vector<std::string>& args, MonitorJobsRequest& request) {
    std::vector<std::string> allowed_flags = {"--json"};
    
    auto validation_result = validate_no_unknown_flags(args, allowed_flags);
    if (!validation_result.success) {
        return validation_result;
    }
    
    auto duplicate_result = validate_no_duplicates(args);
    if (!duplicate_result.success) {
        return duplicate_result;
    }
    
    request.flags.json_output = has_flag(args, "--json");
    
    return MonitorParseResult::ok();
}

MonitorParseResult MonitorArgumentParser::parse_job_args(const std::vector<std::string>& args, MonitorJobRequest& request) {
    std::vector<std::string> allowed_flags = {"--id", "--json"};
    
    auto validation_result = validate_no_unknown_flags(args, allowed_flags);
    if (!validation_result.success) {
        return validation_result;
    }
    
    auto duplicate_result = validate_no_duplicates(args);
    if (!duplicate_result.success) {
        return duplicate_result;
    }
    
    std::string job_id = get_flag_value(args, "--id");
    if (job_id.empty()) {
        return MonitorParseResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Missing required flag: --id"
        );
    }
    
    request.job_id = job_id;
    request.flags.json_output = has_flag(args, "--json");
    
    return MonitorParseResult::ok();
}

MonitorParseResult MonitorArgumentParser::parse_engines_args(const std::vector<std::string>& args, MonitorEnginesRequest& request) {
    std::vector<std::string> allowed_flags = {"--json"};
    
    auto validation_result = validate_no_unknown_flags(args, allowed_flags);
    if (!validation_result.success) {
        return validation_result;
    }
    
    auto duplicate_result = validate_no_duplicates(args);
    if (!duplicate_result.success) {
        return duplicate_result;
    }
    
    request.flags.json_output = has_flag(args, "--json");
    
    return MonitorParseResult::ok();
}

MonitorParseResult MonitorArgumentParser::parse_version_args(const std::vector<std::string>& args, MonitorVersionRequest& request) {
    std::vector<std::string> allowed_flags = {"--json"};
    
    auto validation_result = validate_no_unknown_flags(args, allowed_flags);
    if (!validation_result.success) {
        return validation_result;
    }
    
    auto duplicate_result = validate_no_duplicates(args);
    if (!duplicate_result.success) {
        return duplicate_result;
    }
    
    request.flags.json_output = has_flag(args, "--json");
    
    return MonitorParseResult::ok();
}

bool MonitorArgumentParser::has_flag(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

std::string MonitorArgumentParser::get_flag_value(const std::vector<std::string>& args, const std::string& flag) {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && std::next(it) != args.end()) {
        return *std::next(it);
    }
    return "";
}

MonitorParseResult MonitorArgumentParser::validate_no_unknown_flags(const std::vector<std::string>& args, 
                                                                    const std::vector<std::string>& allowed_flags) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].starts_with("--")) {
            if (std::find(allowed_flags.begin(), allowed_flags.end(), args[i]) == allowed_flags.end()) {
                return MonitorParseResult::error(
                    CliErrorCode::NX_CLI_USAGE_ERROR,
                    "Unknown flag: " + args[i]
                );
            }
        }
    }
    return MonitorParseResult::ok();
}

MonitorParseResult MonitorArgumentParser::validate_no_duplicates(const std::vector<std::string>& args) {
    std::set<std::string> seen_flags;
    
    for (const auto& arg : args) {
        if (arg.starts_with("--")) {
            if (seen_flags.count(arg)) {
                return MonitorParseResult::error(
                    CliErrorCode::NX_CLI_USAGE_ERROR,
                    "Duplicate flag: " + arg
                );
            }
            seen_flags.insert(arg);
        }
    }
    return MonitorParseResult::ok();
}

} // namespace nx::cli