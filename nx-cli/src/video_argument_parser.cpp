#include "video_argument_parser.h"
#include <algorithm>
#include <sstream>
#include <set>
#include <regex>

namespace nx::cli {

CliResult VideoArgumentParser::parse_analyze(const std::vector<std::string>& args, VideoAnalyzeRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--json"
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
    
    // Construct request
    request.input_path = input;
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

CliResult VideoArgumentParser::parse_process(const std::vector<std::string>& args, VideoProcessRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--output", "--scale", "--crop", "--pixel-format", 
        "--color-space", "--range", "--hdr", "--report", "--dry-run", "--json"
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
    
    // Parse video processing flags
    std::string scale_str = get_flag_value(args, "--scale");
    if (!scale_str.empty()) {
        std::string dimensions;
        auto scale_result = parse_scale_dimensions(scale_str, dimensions);
        if (!scale_result.success) {
            return scale_result;
        }
        request.scale_dimensions = dimensions;
    }
    
    std::string crop_str = get_flag_value(args, "--crop");
    if (!crop_str.empty()) {
        std::string parameters;
        auto crop_result = parse_crop_parameters(crop_str, parameters);
        if (!crop_result.success) {
            return crop_result;
        }
        request.crop_parameters = parameters;
    }
    
    std::string pixel_format = get_flag_value(args, "--pixel-format");
    if (!pixel_format.empty()) {
        request.pixel_format = pixel_format;
    }
    
    std::string color_space = get_flag_value(args, "--color-space");
    if (!color_space.empty()) {
        request.color_space = color_space;
    }
    
    std::string range_str = get_flag_value(args, "--range");
    if (!range_str.empty()) {
        auto range = parse_video_range(range_str);
        if (!range) {
            return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid range: " + range_str);
        }
        request.range = *range;
        
        // Range requires color-space
        if (!request.color_space.has_value()) {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--range requires --color-space");
        }
    }
    
    std::string hdr_str = get_flag_value(args, "--hdr");
    if (!hdr_str.empty()) {
        auto hdr = parse_hdr_mode(hdr_str);
        if (!hdr) {
            return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid HDR mode: " + hdr_str);
        }
        request.hdr_mode = *hdr;
    }
    
    // Optional flags
    std::string report = get_flag_value(args, "--report");
    
    // Construct request
    request.input_path = input;
    request.output_path = output;
    if (!report.empty()) {
        request.report_path = report;
    }
    request.flags.dry_run = has_flag(args, "--dry-run");
    request.flags.json_output = has_flag(args, "--json");
    
    // Validate at least one video operation
    if (!request.has_video_operations()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "At least one video operation must be specified");
    }
    
    return CliResult::ok();
}

CliResult VideoArgumentParser::parse_verify(const std::vector<std::string>& args, VideoVerifyRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--output", "--json"
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
    
    // Construct request
    request.input_path = input;
    request.output_path = output;
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

std::optional<VideoRange> VideoArgumentParser::parse_video_range(const std::string& value) {
    if (value == "full") return VideoRange::Full;
    if (value == "limited") return VideoRange::Limited;
    return std::nullopt;
}

std::optional<HdrMode> VideoArgumentParser::parse_hdr_mode(const std::string& value) {
    if (value == "passthrough") return HdrMode::Passthrough;
    if (value == "strip") return HdrMode::Strip;
    return std::nullopt;
}

CliResult VideoArgumentParser::parse_scale_dimensions(const std::string& value, std::string& dimensions) {
    // Validate format: WIDTHxHEIGHT
    std::regex scale_regex(R"(^(\d+)x(\d+)$)");
    if (!std::regex_match(value, scale_regex)) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Invalid scale format, expected WIDTHxHEIGHT: " + value);
    }
    dimensions = value;
    return CliResult::ok();
}

CliResult VideoArgumentParser::parse_crop_parameters(const std::string& value, std::string& parameters) {
    // Validate format: WIDTHxHEIGHT+X+Y
    std::regex crop_regex(R"(^(\d+)x(\d+)\+(\d+)\+(\d+)$)");
    if (!std::regex_match(value, crop_regex)) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Invalid crop format, expected WIDTHxHEIGHT+X+Y: " + value);
    }
    parameters = value;
    return CliResult::ok();
}

std::string VideoArgumentParser::get_flag_value(const std::vector<std::string>& args, const std::string& flag) {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && (it + 1) != args.end()) {
        return *(it + 1);
    }
    return "";
}

bool VideoArgumentParser::has_flag(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

CliResult VideoArgumentParser::validate_flags(const std::vector<std::string>& args, const std::vector<std::string>& allowed_flags) {
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