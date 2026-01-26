#pragma once

#include "cli_types.h"
#include <vector>
#include <string>
#include <optional>

namespace nx::cli {

class ArgumentParser {
public:
    // Parse nx convert transcode arguments
    static CliResult parse_transcode(const std::vector<std::string>& args, TranscodeRequest& request);
    
    // Parse nx convert analyze arguments  
    static CliResult parse_analyze(const std::vector<std::string>& args, std::string& input_path, bool& json_output);
    
    // Parse nx convert verify arguments
    static CliResult parse_verify(const std::vector<std::string>& args, std::string& input_path, std::string& output_path, bool& json_output);

private:
    // Enum parsers (static registry validation only)
    static std::optional<ContainerType> parse_container(const std::string& value);
    static std::optional<VideoPolicy> parse_video_policy(const std::string& value, std::string& codec);
    static std::optional<AudioPolicy> parse_audio_policy(const std::string& value, std::string& codec);
    static std::optional<SubtitlePolicy> parse_subtitle_policy(const std::string& value);
    static std::optional<MetadataPolicy> parse_metadata_policy(const std::string& value);
    
    // Flag validation
    static bool is_valid_flag(const std::string& flag);
    static std::string get_flag_value(const std::vector<std::string>& args, const std::string& flag);
    static bool has_flag(const std::vector<std::string>& args, const std::string& flag);
    static CliResult validate_flags(const std::vector<std::string>& args, const std::vector<std::string>& allowed_flags);
};

} // namespace nx::cli