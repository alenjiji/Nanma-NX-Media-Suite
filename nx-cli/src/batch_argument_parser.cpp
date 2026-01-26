#include "batch_argument_parser.h"
#include <algorithm>
#include <set>

namespace nx::cli {

BatchParseResult BatchArgumentParser::parse_run_args(const std::vector<std::string>& args, BatchRunRequest& request) {
    std::vector<std::string> allowed_flags = {"--file", "--dry-run", "--json"};
    
    auto validation_result = validate_no_unknown_flags(args, allowed_flags);
    if (!validation_result.success) {
        return validation_result;
    }
    
    auto duplicate_result = validate_no_duplicates(args);
    if (!duplicate_result.success) {
        return duplicate_result;
    }
    
    std::string file_path = get_flag_value(args, "--file");
    if (file_path.empty()) {
        return BatchParseResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Missing required flag: --file"
        );
    }
    
    request.batch_file = file_path;
    request.flags.dry_run = has_flag(args, "--dry-run");
    request.flags.json_output = has_flag(args, "--json");
    
    return BatchParseResult::ok();
}

BatchParseResult BatchArgumentParser::parse_validate_args(const std::vector<std::string>& args, BatchValidateRequest& request) {
    std::vector<std::string> allowed_flags = {"--file", "--json"};
    
    auto validation_result = validate_no_unknown_flags(args, allowed_flags);
    if (!validation_result.success) {
        return validation_result;
    }
    
    auto duplicate_result = validate_no_duplicates(args);
    if (!duplicate_result.success) {
        return duplicate_result;
    }
    
    std::string file_path = get_flag_value(args, "--file");
    if (file_path.empty()) {
        return BatchParseResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Missing required flag: --file"
        );
    }
    
    request.batch_file = file_path;
    request.flags.json_output = has_flag(args, "--json");
    
    return BatchParseResult::ok();
}

BatchParseResult BatchArgumentParser::parse_summary_args(const std::vector<std::string>& args, BatchSummaryRequest& request) {
    std::vector<std::string> allowed_flags = {"--file", "--json"};
    
    auto validation_result = validate_no_unknown_flags(args, allowed_flags);
    if (!validation_result.success) {
        return validation_result;
    }
    
    auto duplicate_result = validate_no_duplicates(args);
    if (!duplicate_result.success) {
        return duplicate_result;
    }
    
    std::string file_path = get_flag_value(args, "--file");
    if (file_path.empty()) {
        return BatchParseResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Missing required flag: --file"
        );
    }
    
    request.batch_file = file_path;
    request.flags.json_output = has_flag(args, "--json");
    
    return BatchParseResult::ok();
}

bool BatchArgumentParser::has_flag(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

std::string BatchArgumentParser::get_flag_value(const std::vector<std::string>& args, const std::string& flag) {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && std::next(it) != args.end()) {
        return *std::next(it);
    }
    return "";
}

BatchParseResult BatchArgumentParser::validate_no_unknown_flags(const std::vector<std::string>& args, 
                                                                const std::vector<std::string>& allowed_flags) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].starts_with("--")) {
            if (std::find(allowed_flags.begin(), allowed_flags.end(), args[i]) == allowed_flags.end()) {
                return BatchParseResult::error(
                    CliErrorCode::NX_CLI_USAGE_ERROR,
                    "Unknown flag: " + args[i]
                );
            }
        }
    }
    return BatchParseResult::ok();
}

BatchParseResult BatchArgumentParser::validate_no_duplicates(const std::vector<std::string>& args) {
    std::set<std::string> seen_flags;
    
    for (const auto& arg : args) {
        if (arg.starts_with("--")) {
            if (seen_flags.count(arg)) {
                return BatchParseResult::error(
                    CliErrorCode::NX_CLI_USAGE_ERROR,
                    "Duplicate flag: " + arg
                );
            }
            seen_flags.insert(arg);
        }
    }
    return BatchParseResult::ok();
}

} // namespace nx::cli