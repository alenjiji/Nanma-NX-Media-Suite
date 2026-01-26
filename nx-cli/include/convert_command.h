#pragma once

#include "cli_types.h"
#include <vector>
#include <string>

namespace nx::cli {

class ConvertCommand {
public:
    // Execute nx convert <operation>
    static CliResult execute(const std::vector<std::string>& args);

private:
    // Operation handlers
    static CliResult handle_transcode(const std::vector<std::string>& args);
    static CliResult handle_analyze(const std::vector<std::string>& args);
    static CliResult handle_verify(const std::vector<std::string>& args);
    
    // Engine invocation (adapter only)
    static CliResult invoke_transcode_engine(const TranscodeRequest& request);
    static CliResult invoke_analyze_engine(const std::string& input_path, bool json_output);
    static CliResult invoke_verify_engine(const std::string& input_path, const std::string& output_path, bool json_output);
};

} // namespace nx::cli