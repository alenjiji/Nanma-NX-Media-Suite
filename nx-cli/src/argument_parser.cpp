#include "argument_parser.h"
#include <algorithm>
#include <sstream>

namespace nx::cli {

CliResult ArgumentParser::parse_transcode(const std::vector<std::string>& args, TranscodeRequest& request) {
    // Validate flags first
    std::vector<std::string> allowed_flags = {
        "--input", "--output", "--container", "--video", "--audio", 
        "--subtitle", "--metadata", "--dry-run", "--json"
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
    
    std::string container_str = get_flag_value(args, "--container");
    if (container_str.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --container");
    }
    
    std::string video_str = get_flag_value(args, "--video");
    if (video_str.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --video");
    }
    
    std::string audio_str = get_flag_value(args, "--audio");
    if (audio_str.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --audio");
    }
    
    // Parse enums
    auto container = parse_container(container_str);
    if (!container) {
        return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid container: " + container_str);
    }
    
    std::string video_codec;
    auto video_policy = parse_video_policy(video_str, video_codec);
    if (!video_policy) {
        return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid video policy: " + video_str);
    }
    
    std::string audio_codec;
    auto audio_policy = parse_audio_policy(audio_str, audio_codec);
    if (!audio_policy) {
        return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid audio policy: " + audio_str);
    }
    
    // Optional flags - leave unspecified if not provided
    std::string subtitle_str = get_flag_value(args, "--subtitle");
    if (!subtitle_str.empty()) {
        auto parsed_subtitle = parse_subtitle_policy(subtitle_str);
        if (!parsed_subtitle) {
            return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid subtitle policy: " + subtitle_str);
        }
        request.subtitle_policy = *parsed_subtitle;
    }
    
    std::string metadata_str = get_flag_value(args, "--metadata");
    if (!metadata_str.empty()) {
        auto parsed_metadata = parse_metadata_policy(metadata_str);
        if (!parsed_metadata) {
            return CliResult::error(CliErrorCode::NX_CLI_ENUM_ERROR, "Invalid metadata policy: " + metadata_str);
        }
        request.metadata_policy = *parsed_metadata;
    }
    
    // Construct request
    request.input_path = input;
    request.output_path = output;
    request.target_container = *container;
    request.video_policy = *video_policy;
    request.video_codec = video_codec;
    request.audio_policy = *audio_policy;
    request.audio_codec = audio_codec;
    // subtitle_policy and metadata_policy are already set above or left unspecified
    request.flags.dry_run = has_flag(args, "--dry-run");
    request.flags.json_output = has_flag(args, "--json");
    
    return CliResult::ok();
}

CliResult ArgumentParser::parse_analyze(const std::vector<std::string>& args, std::string& input_path, bool& json_output) {
    input_path = get_flag_value(args, "--input");
    if (input_path.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --input");
    }
    
    json_output = has_flag(args, "--json");
    return CliResult::ok();
}

CliResult ArgumentParser::parse_verify(const std::vector<std::string>& args, std::string& input_path, std::string& output_path, bool& json_output) {
    input_path = get_flag_value(args, "--input");
    if (input_path.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --input");
    }
    
    output_path = get_flag_value(args, "--output");
    if (output_path.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing required flag: --output");
    }
    
    json_output = has_flag(args, "--json");
    return CliResult::ok();
}

std::optional<ContainerType> ArgumentParser::parse_container(const std::string& value) {
    if (value == "mp4") return ContainerType::MP4;
    if (value == "mov") return ContainerType::MOV;
    if (value == "mkv") return ContainerType::MKV;
    if (value == "avi") return ContainerType::AVI;
    return std::nullopt;
}

std::optional<VideoPolicy> ArgumentParser::parse_video_policy(const std::string& value, std::string& codec) {
    if (value == "passthrough") {
        codec.clear();
        return VideoPolicy::Passthrough;
    }
    
    if (value.substr(0, 7) == "encode:") {
        codec = value.substr(7);
        if (codec.empty()) {
            return std::nullopt; // encode: without codec is forbidden
        }
        return VideoPolicy::Encode;
    }
    
    return std::nullopt;
}

std::optional<AudioPolicy> ArgumentParser::parse_audio_policy(const std::string& value, std::string& codec) {
    if (value == "passthrough") {
        codec.clear();
        return AudioPolicy::Passthrough;
    }
    
    if (value.substr(0, 7) == "encode:") {
        codec = value.substr(7);
        if (codec.empty()) {
            return std::nullopt; // encode: without codec is forbidden
        }
        return AudioPolicy::Encode;
    }
    
    return std::nullopt;
}

std::optional<SubtitlePolicy> ArgumentParser::parse_subtitle_policy(const std::string& value) {
    if (value == "passthrough") return SubtitlePolicy::Passthrough;
    if (value == "drop") return SubtitlePolicy::Drop;
    return std::nullopt;
}

std::optional<MetadataPolicy> ArgumentParser::parse_metadata_policy(const std::string& value) {
    if (value == "passthrough") return MetadataPolicy::Passthrough;
    if (value == "strip") return MetadataPolicy::Strip;
    return std::nullopt;
}

std::string ArgumentParser::get_flag_value(const std::vector<std::string>& args, const std::string& flag) {
    auto it = std::find(args.begin(), args.end(), flag);
    if (it != args.end() && (it + 1) != args.end()) {
        return *(it + 1);
    }
    return "";
}

bool ArgumentParser::has_flag(const std::vector<std::string>& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

CliResult ArgumentParser::validate_flags(const std::vector<std::string>& args, const std::vector<std::string>& allowed_flags) {
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