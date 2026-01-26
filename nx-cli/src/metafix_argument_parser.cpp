#include "metafix_argument_parser.h"
#include <algorithm>
#include <sstream>
#include <set>

namespace nx::cli {

CliResult MetaFixArgumentParser::parse_repair(const std::vector<std::string>& args, RepairRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--output", "--mode", "--allow-essence-modification", 
        "--report", "--dry-run", "--json"
    };
    auto flag_result = validate_flags(args, allowed_flags);
    if (!flag_result.success) {
        return flag_result;
    }
    
    // Required flags validation
    std::string input = get_flag_value(args, "--input");
    if (input.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --input");
    }
    
    std::string output = get_flag_value(args, "--output");
    if (output.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --output");
    }
    
    std::string mode_str = get_flag_value(args, "--mode");
    if (mode_str.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --mode");
    }
    
    // Parse repair mode
    auto mode = parse_repair_mode(mode_str);
    if (!mode) {
        return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid repair mode: " + mode_str);
    }
    
    // Optional flags
    std::string report = get_flag_value(args, "--report");
    
    // Construct request
    request.input_path = input;
    request.output_path = output;
    request.mode = *mode;
    request.allow_essence_modification = has_flag(args, "--allow-essence-modification");
    if (!report.empty()) {
        request.report_path = report;
    }
    request.flags.dry_run = has_flag(args, "--dry-run");
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

CliResult MetaFixArgumentParser::parse_validate(const std::vector<std::string>& args, ValidateRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--profile", "--json"
    };
    auto flag_result = validate_flags(args, allowed_flags);
    if (!flag_result.success) {
        return flag_result;
    }
    
    // Required flags validation
    std::string input = get_flag_value(args, "--input");
    if (input.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --input");
    }
    
    // Optional profile
    std::string profile_str = get_flag_value(args, "--profile");
    if (!profile_str.empty()) {
        auto profile = parse_validation_profile(profile_str);
        if (!profile) {
            return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid validation profile: " + profile_str);
        }
        request.profile = *profile;
    }
    
    // Construct request
    request.input_path = input;
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

CliResult MetaFixArgumentParser::parse_metadata_copy(const std::vector<std::string>& args, MetadataCopyRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--source", "--target", "--categories", "--overwrite", "--report", "--json"
    };
    auto flag_result = validate_flags(args, allowed_flags);
    if (!flag_result.success) {
        return flag_result;
    }
    
    // Required flags validation
    std::string source = get_flag_value(args, "--source");
    if (source.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --source");
    }
    
    std::string target = get_flag_value(args, "--target");
    if (target.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --target");
    }
    
    std::string categories_str = get_flag_value(args, "--categories");
    if (categories_str.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --categories");
    }
    
    // Parse categories
    std::vector<MetadataCategory> categories;
    auto categories_result = parse_metadata_categories(categories_str, categories);
    if (!categories_result.success) {
        return categories_result;
    }
    
    // Optional flags
    std::string report = get_flag_value(args, "--report");
    
    // Construct request
    request.source_path = source;
    request.target_path = target;
    request.categories = categories;
    request.overwrite = has_flag(args, "--overwrite");
    if (!report.empty()) {
        request.report_path = report;
    }
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

CliResult MetaFixArgumentParser::parse_metadata_merge(const std::vector<std::string>& args, MetadataMergeRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--inputs", "--output", "--precedence", "--report", "--json"
    };
    auto flag_result = validate_flags(args, allowed_flags);
    if (!flag_result.success) {
        return flag_result;
    }
    
    // Required flags validation
    std::string inputs_str = get_flag_value(args, "--inputs");
    if (inputs_str.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --inputs");
    }
    
    std::string output = get_flag_value(args, "--output");
    if (output.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --output");
    }
    
    std::string precedence_str = get_flag_value(args, "--precedence");
    if (precedence_str.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --precedence");
    }
    
    // Parse inputs and precedence
    std::vector<std::string> inputs;
    auto inputs_result = parse_input_list(inputs_str, inputs);
    if (!inputs_result.success) {
        return inputs_result;
    }
    
    std::vector<std::string> precedence;
    auto precedence_result = parse_precedence_list(precedence_str, precedence);
    if (!precedence_result.success) {
        return precedence_result;
    }
    
    // Validate precedence matches inputs
    if (inputs.size() != precedence.size()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Precedence list must match input count");
    }
    
    // Check for duplicates in precedence list
    std::set<std::string> unique_precedence(precedence.begin(), precedence.end());
    if (unique_precedence.size() != precedence.size()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Precedence list contains duplicates");
    }
    
    // Optional flags
    std::string report = get_flag_value(args, "--report");
    
    // Construct request
    request.input_paths = inputs;
    request.output_path = output;
    request.precedence_order = precedence;
    if (!report.empty()) {
        request.report_path = report;
    }
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

CliResult MetaFixArgumentParser::parse_metadata_normalize(const std::vector<std::string>& args, MetadataNormalizeRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--output", "--schema", "--report", "--json"
    };
    auto flag_result = validate_flags(args, allowed_flags);
    if (!flag_result.success) {
        return flag_result;
    }
    
    // Required flags validation
    std::string input = get_flag_value(args, "--input");
    if (input.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --input");
    }
    
    std::string output = get_flag_value(args, "--output");
    if (output.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --output");
    }
    
    std::string schema = get_flag_value(args, "--schema");
    if (schema.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --schema");
    }
    
    // Optional flags
    std::string report = get_flag_value(args, "--report");
    
    // Construct request
    request.input_path = input;
    request.output_path = output;
    request.schema_id = schema;
    if (!report.empty()) {
        request.report_path = report;
    }
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

std::optional<RepairMode> MetaFixArgumentParser::parse_repair_mode(const std::string& value) {
    if (value == "header-rebuild") return RepairMode::HeaderRebuild;
    if (value == "index-regenerate") return RepairMode::IndexRegenerate;
    if (value == "atom-repair") return RepairMode::AtomRepair;
    if (value == "interleave-correct") return RepairMode::InterleaveCorrect;
    return std::nullopt;
}

std::optional<ValidationProfile> MetaFixArgumentParser::parse_validation_profile(const std::string& value) {
    if (value == "container") return ValidationProfile::Container;
    if (value == "metadata") return ValidationProfile::Metadata;
    if (value == "broadcast") return ValidationProfile::Broadcast;
    return std::nullopt;
}

std::optional<MetadataCategory> MetaFixArgumentParser::parse_metadata_category(const std::string& value) {
    if (value == "technical") return MetadataCategory::Technical;
    if (value == "descriptive") return MetadataCategory::Descriptive;
    if (value == "rights") return MetadataCategory::Rights;
    if (value == "chapters") return MetadataCategory::Chapters;
    if (value == "artwork") return MetadataCategory::Artwork;
    return std::nullopt;
}

CliResult MetaFixArgumentParser::parse_metadata_categories(const std::string& value, std::vector<MetadataCategory>& categories) {
    if (value.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Categories list cannot be empty");
    }
    
    std::stringstream ss(value);
    std::string category;
    
    while (std::getline(ss, category, ',')) {
        auto parsed_category = parse_metadata_category(category);
        if (!parsed_category) {
            return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid metadata category: " + category);
        }
        categories.push_back(*parsed_category);
    }
    
    return CliResult::ok();
}

CliResult MetaFixArgumentParser::parse_input_list(const std::string& value, std::vector<std::string>& inputs) {
    if (value.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Input list cannot be empty");
    }
    
    std::stringstream ss(value);
    std::string input;
    
    while (std::getline(ss, input, ',')) {
        if (input.empty()) {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Empty input path in list");
        }
        inputs.push_back(input);
    }
    
    return CliResult::ok();
}

CliResult MetaFixArgumentParser::parse_precedence_list(const std::string& value, std::vector<std::string>& precedence) {
    if (value.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Precedence list cannot be empty");
    }
    
    std::stringstream ss(value);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        if (item.empty()) {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Empty precedence item in list");
        }
        precedence.push_back(item);
    }
    
    return CliResult::ok();
}

std::string MetaFixArgumentParser::get_flag_value(const std::vector<std::string>& args, const std::string& flag) {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && (it + 1) != args.end()) {
        return *(it + 1);
    }
    return "";
}

bool MetaFixArgumentParser::has_flag(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

CliResult MetaFixArgumentParser::validate_flags(const std::vector<std::string>& args, const std::vector<std::string>& allowed_flags) {
    std::vector<std::string> seen_flags;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg.substr(0, 2) == "--") {
            // Check if flag is allowed
            if (std::find(allowed_flags.begin(), allowed_flags.end(), arg) == allowed_flags.end()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown flag: " + arg);
            }
            
            // Check for duplicates
            if (std::find(seen_flags.begin(), seen_flags.end(), arg) != seen_flags.end()) {
                return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Duplicate flag: " + arg);
            }
            
            seen_flags.push_back(arg);
        }
    }
    
    return CliResult::ok();
}

} // namespace nx::cli