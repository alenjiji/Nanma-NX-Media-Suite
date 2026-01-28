// PHASE 15 INVARIANT:
// This adapter is a stateless CLI bridge.
// It adds no defaults, no retries, no inference.
// All parameters map 1:1 to CLI arguments.
// If a value is missing, this is a caller error.

#pragma once

#include <string>

namespace nx::ui::adapters {

struct VideoAnalyzeDto {};
struct VideoProcessDto {};
struct VideoVerifyDto {};

template <typename Dto>
struct CliTypedResult {
    int exit_code;
    std::string raw_json;
};

struct VideoAnalyzeArgs {
    std::string input_path;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct VideoProcessArgs {
    std::string input_path;
    std::string output_path;
    std::string color_space;
    std::string encoding_preset;
    bool gpu_acceleration;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct VideoVerifyArgs {
    std::string input_path;
    std::string output_path;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

class VideoAdapter {
public:
    static CliTypedResult<VideoAnalyzeDto> analyze(const VideoAnalyzeArgs& args);
    static CliTypedResult<VideoProcessDto> process(const VideoProcessArgs& args);
    static CliTypedResult<VideoVerifyDto> verify(const VideoVerifyArgs& args);
};

} // namespace nx::ui::adapters