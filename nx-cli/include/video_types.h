#pragma once

#include <string>
#include <vector>
#include <optional>

namespace nx::cli {

// Video Range Types (Explicit)
enum class VideoRange {
    Full,
    Limited
};

// HDR Mode Types (Explicit)
enum class HdrMode {
    Passthrough,
    Strip
};

// Video Request Types (Maps 1:1 to Engine)
struct VideoAnalyzeRequest {
    std::string input_path;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

struct VideoProcessRequest {
    std::string input_path;
    std::string output_path;
    
    // Video Processing Nodes (Explicit)
    std::optional<std::string> scale_dimensions;     // e.g. "1920x1080"
    std::optional<std::string> crop_parameters;     // e.g. "1920x800+0+140"
    std::optional<std::string> pixel_format;        // e.g. "yuv420p"
    std::optional<std::string> color_space;         // e.g. "bt709"
    std::optional<VideoRange> range;
    std::optional<HdrMode> hdr_mode;
    
    std::optional<std::string> report_path;
    
    struct Flags {
        bool dry_run = false;
        bool json_output = false;
    } flags;
    
    // Validation helper
    bool has_video_operations() const {
        return scale_dimensions.has_value() || 
               crop_parameters.has_value() || 
               pixel_format.has_value() || 
               color_space.has_value() || 
               range.has_value() || 
               hdr_mode.has_value();
    }
};

struct VideoVerifyRequest {
    std::string input_path;
    std::string output_path;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

} // namespace nx::cli