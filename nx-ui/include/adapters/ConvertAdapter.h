// PHASE 15 INVARIANT:
// This adapter is a stateless CLI bridge.
// It adds no defaults, no retries, no inference.
// All parameters map 1:1 to CLI arguments.
// If a value is missing, this is a caller error.

#pragma once

#include <string>

namespace nx::ui::adapters {

struct ConvertTranscodeDto {};
struct ConvertAnalyzeDto {};
struct ConvertVerifyDto {};

template <typename Dto>
struct CliTypedResult {
    int exit_code;
    std::string raw_json;
};

struct ConvertTranscodeArgs {
    std::string input_path;
    std::string output_path;
    std::string format;
    std::string codec;
    std::string quality;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct ConvertAnalyzeArgs {
    std::string input_path;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct ConvertVerifyArgs {
    std::string input_path;
    std::string output_path;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

class ConvertAdapter {
public:
    static CliTypedResult<ConvertTranscodeDto> transcode(const ConvertTranscodeArgs& args);
    static CliTypedResult<ConvertAnalyzeDto> analyze(const ConvertAnalyzeArgs& args);
    static CliTypedResult<ConvertVerifyDto> verify(const ConvertVerifyArgs& args);
};

} // namespace nx::ui::adapters