#pragma once

#include "cli_types.h"
#include "video_types.h"
#include <vector>
#include <string>
#include <optional>

namespace nx::cli {

class VideoArgumentParser {
public:
    // Parse nx video analyze arguments
    static CliResult parse_analyze(const std::vector<std::string>& args, VideoAnalyzeRequest& request);
    
    // Parse nx video process arguments
    static CliResult parse_process(const std::vector<std::string>& args, VideoProcessRequest& request);
    
    // Parse nx video verify arguments
    static CliResult parse_verify(const std::vector<std::string>& args, VideoVerifyRequest& request);

private:
    // Enum parsers (static registry validation only)
    static std::optional<VideoRange> parse_video_range(const std::string& value);
    static std::optional<HdrMode> parse_hdr_mode(const std::string& value);
    static CliResult parse_scale_dimensions(const std::string& value, std::string& dimensions);
    static CliResult parse_crop_parameters(const std::string& value, std::string& parameters);
    
    // Utility functions
    static std::string get_flag_value(const std::vector<std::string>& args, const std::string& flag);
    static bool has_flag(const std::vector<std::string>& args, const std::string& flag);
    static CliResult validate_flags(const std::vector<std::string>& args, const std::vector<std::string>& allowed_flags);
};

} // namespace nx::cli