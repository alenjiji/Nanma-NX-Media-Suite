#pragma once

#include "cli_types.h"
#include "audio_types.h"
#include <vector>
#include <string>

namespace nx::cli {

class AudioCommand {
public:
    // Execute nx audio <operation>
    static CliResult execute(const std::vector<std::string>& args);

private:
    // Operation handlers
    static CliResult handle_measure(const std::vector<std::string>& args);
    static CliResult handle_process(const std::vector<std::string>& args);
    static CliResult handle_verify(const std::vector<std::string>& args);
    
    // Engine invocation (adapter only)
    static CliResult invoke_measure_engine(const AudioMeasureRequest& request);
    static CliResult invoke_process_engine(const AudioProcessRequest& request);
    static CliResult invoke_verify_engine(const AudioVerifyRequest& request);
};

} // namespace nx::cli