#pragma once

#include "cli_types.h"
#include "audio_types.h"
#include <vector>
#include <string>
#include <optional>

namespace nx::cli {

class AudioArgumentParser {
public:
    // Parse nx audio measure arguments
    static CliResult parse_measure(const std::vector<std::string>& args, AudioMeasureRequest& request);
    
    // Parse nx audio process arguments
    static CliResult parse_process(const std::vector<std::string>& args, AudioProcessRequest& request);
    
    // Parse nx audio verify arguments
    static CliResult parse_verify(const std::vector<std::string>& args, AudioVerifyRequest& request);

private:
    // Enum parsers (static registry validation only)
    static std::optional<LoudnessStandard> parse_loudness_standard(const std::string& value);
    static std::optional<DitherType> parse_dither_type(const std::string& value);
    static CliResult parse_loudness_target(const std::string& value, std::string& target);
    static CliResult parse_sample_rate(const std::string& value, int& rate);
    static CliResult parse_gain_db(const std::string& value, double& gain);
    static CliResult parse_bit_depth(const std::string& value, int& depth);
    
    // Utility functions
    static std::string get_flag_value(const std::vector<std::string>& args, const std::string& flag);
    static bool has_flag(const std::vector<std::string>& args, const std::string& flag);
    static CliResult validate_flags(const std::vector<std::string>& args, const std::vector<std::string>& allowed_flags);
};

} // namespace nx::cli