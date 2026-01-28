// PHASE 15 INVARIANT:
// This adapter is a stateless CLI bridge.
// It adds no defaults, no retries, no inference.
// All parameters map 1:1 to CLI arguments.
// If a value is missing, this is a caller error.

#pragma once

#include <string>

namespace nx::ui::adapters {

struct AudioMeasureDto {};
struct AudioProcessDto {};
struct AudioVerifyDto {};

template <typename Dto>
struct CliTypedResult {
    int exit_code;
    std::string raw_json;
};

struct AudioMeasureArgs {
    std::string input_path;
    std::string measurement_standard;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct AudioProcessArgs {
    std::string input_path;
    std::string output_path;
    std::string loudness_target;
    std::string sample_rate;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct AudioVerifyArgs {
    std::string input_path;
    std::string output_path;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

class AudioAdapter {
public:
    static CliTypedResult<AudioMeasureDto> measure(const AudioMeasureArgs& args);
    static CliTypedResult<AudioProcessDto> process(const AudioProcessArgs& args);
    static CliTypedResult<AudioVerifyDto> verify(const AudioVerifyArgs& args);
};

} // namespace nx::ui::adapters