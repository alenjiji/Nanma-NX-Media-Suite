#include "audio_argument_parser.h"
#include <algorithm>
#include <sstream>
#include <set>

namespace nx::cli {

CliResult AudioArgumentParser::parse_measure(const std::vector<std::string>& args, AudioMeasureRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--standard", "--report", "--json"
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
    
    std::string standard_str = get_flag_value(args, "--standard");
    if (standard_str.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --standard");
    }
    
    // Parse loudness standard
    auto standard = parse_loudness_standard(standard_str);
    if (!standard) {
        return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid loudness standard: " + standard_str);
    }
    
    // Optional flags
    std::string report = get_flag_value(args, "--report");
    
    // Construct request
    request.input_path = input;
    request.standard = *standard;
    if (!report.empty()) {
        request.report_path = report;
    }
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

CliResult AudioArgumentParser::parse_process(const std::vector<std::string>& args, AudioProcessRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--output", "--src", "--gain", "--loudness-normalize", 
        "--bit-depth", "--dither", "--report", "--dry-run", "--json"
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
    
    // Parse DSP flags
    std::string src_str = get_flag_value(args, "--src");
    if (!src_str.empty()) {
        int rate;
        auto src_result = parse_sample_rate(src_str, rate);
        if (!src_result.success) {
            return src_result;
        }
        request.src_rate = rate;
    }
    
    std::string gain_str = get_flag_value(args, "--gain");
    if (!gain_str.empty()) {
        double gain;
        auto gain_result = parse_gain_db(gain_str, gain);
        if (!gain_result.success) {
            return gain_result;
        }
        request.gain_db = gain;
    }
    
    std::string loudness_str = get_flag_value(args, "--loudness-normalize");
    if (!loudness_str.empty()) {
        std::string target;
        auto loudness_result = parse_loudness_target(loudness_str, target);
        if (!loudness_result.success) {
            return loudness_result;
        }
        request.loudness_target = target;
    }
    
    std::string bit_depth_str = get_flag_value(args, "--bit-depth");
    if (!bit_depth_str.empty()) {
        int depth;
        auto depth_result = parse_bit_depth(bit_depth_str, depth);
        if (!depth_result.success) {
            return depth_result;
        }
        request.bit_depth = depth;
    }
    
    std::string dither_str = get_flag_value(args, "--dither");
    if (!dither_str.empty()) {
        auto dither = parse_dither_type(dither_str);
        if (!dither) {
            return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid dither type: " + dither_str);
        }
        request.dither_type = *dither;
        
        // Dither requires bit-depth
        if (!request.bit_depth.has_value()) {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "--dither requires --bit-depth");
        }
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
    
    // Validate at least one DSP operation
    if (!request.has_dsp_operations()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "At least one DSP operation must be specified");
    }
    
    return CliResult::ok();
}

CliResult AudioArgumentParser::parse_verify(const std::vector<std::string>& args, AudioVerifyRequest& request) {
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

std::optional<LoudnessStandard> AudioArgumentParser::parse_loudness_standard(const std::string& value) {
    if (value == "ebu-r128") return LoudnessStandard::EbuR128;
    if (value == "itu-bs1770") return LoudnessStandard::ItuBs1770;
    if (value == "atsc-a85") return LoudnessStandard::AtscA85;
    return std::nullopt;
}

std::optional<DitherType> AudioArgumentParser::parse_dither_type(const std::string& value) {
    if (value == "triangular") return DitherType::Triangular;
    if (value == "rectangular") return DitherType::Rectangular;
    if (value == "noise-shaped") return DitherType::NoiseShaped;
    return std::nullopt;
}

CliResult AudioArgumentParser::parse_loudness_target(const std::string& value, std::string& target) {
    // Must include LUFS unit
    if (value.find("LUFS") == std::string::npos) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Loudness target must include LUFS unit (e.g. -23LUFS)");
    }
    target = value;
    return CliResult::ok();
}

CliResult AudioArgumentParser::parse_sample_rate(const std::string& value, int& rate) {
    try {
        rate = std::stoi(value);
        if (rate <= 0) {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Sample rate must be positive integer");
        }
    } catch (const std::exception&) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Invalid sample rate: " + value);
    }
    return CliResult::ok();
}

CliResult AudioArgumentParser::parse_gain_db(const std::string& value, double& gain) {
    try {
        gain = std::stod(value);
    } catch (const std::exception&) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Invalid gain value: " + value);
    }
    return CliResult::ok();
}

CliResult AudioArgumentParser::parse_bit_depth(const std::string& value, int& depth) {
    try {
        depth = std::stoi(value);
        if (depth <= 0) {
            return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Bit depth must be positive integer");
        }
    } catch (const std::exception&) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Invalid bit depth: " + value);
    }
    return CliResult::ok();
}

std::string AudioArgumentParser::get_flag_value(const std::vector<std::string>& args, const std::string& flag) {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && (it + 1) != args.end()) {
        return *(it + 1);
    }
    return "";
}

bool AudioArgumentParser::has_flag(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

CliResult AudioArgumentParser::validate_flags(const std::vector<std::string>& args, const std::vector<std::string>& allowed_flags) {
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