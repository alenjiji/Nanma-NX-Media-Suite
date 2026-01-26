#pragma once

#include "cli_types.h"
#include "video_types.h"
#include <vector>
#include <string>

namespace nx::cli {

class VideoCommand {
public:
    // Execute nx video <operation>
    static CliResult execute(const std::vector<std::string>& args);

private:
    // Operation handlers
    static CliResult handle_analyze(const std::vector<std::string>& args);
    static CliResult handle_process(const std::vector<std::string>& args);
    static CliResult handle_verify(const std::vector<std::string>& args);
    
    // Engine invocation (adapter only)
    static CliResult invoke_analyze_engine(const VideoAnalyzeRequest& request);
    static CliResult invoke_process_engine(const VideoProcessRequest& request);
    static CliResult invoke_verify_engine(const VideoVerifyRequest& request);
};

} // namespace nx::cli