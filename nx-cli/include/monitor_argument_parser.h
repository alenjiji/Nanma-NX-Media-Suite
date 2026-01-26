#pragma once

#include "cli_types.h"
#include "monitor_types.h"
#include <vector>
#include <string>

namespace nx::cli {

struct MonitorParseResult {
    CliErrorCode error_code;
    std::string message;
    bool success;
    
    static MonitorParseResult ok() {
        return {CliErrorCode::NONE, "", true};
    }
    
    static MonitorParseResult error(CliErrorCode code, const std::string& msg) {
        return {code, msg, false};
    }
};

class MonitorArgumentParser {
public:
    /**
     * Parse arguments for monitor status operation
     * Optional: --json
     */
    static MonitorParseResult parse_status_args(const std::vector<std::string>& args, MonitorStatusRequest& request);
    
    /**
     * Parse arguments for monitor jobs operation
     * Optional: --json
     */
    static MonitorParseResult parse_jobs_args(const std::vector<std::string>& args, MonitorJobsRequest& request);
    
    /**
     * Parse arguments for monitor job operation
     * Required: --id <job-id>
     * Optional: --json
     */
    static MonitorParseResult parse_job_args(const std::vector<std::string>& args, MonitorJobRequest& request);
    
    /**
     * Parse arguments for monitor engines operation
     * Optional: --json
     */
    static MonitorParseResult parse_engines_args(const std::vector<std::string>& args, MonitorEnginesRequest& request);
    
    /**
     * Parse arguments for monitor version operation
     * Optional: --json
     */
    static MonitorParseResult parse_version_args(const std::vector<std::string>& args, MonitorVersionRequest& request);

private:
    static bool has_flag(const std::vector<std::string>& args, const std::string& flag);
    static std::string get_flag_value(const std::vector<std::string>& args, const std::string& flag);
    static MonitorParseResult validate_no_unknown_flags(const std::vector<std::string>& args, 
                                                        const std::vector<std::string>& allowed_flags);
    static MonitorParseResult validate_no_duplicates(const std::vector<std::string>& args);
};

} // namespace nx::cli