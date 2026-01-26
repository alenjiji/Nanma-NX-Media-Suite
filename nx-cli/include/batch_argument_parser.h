#pragma once

#include "cli_types.h"
#include "batch_types.h"
#include <vector>
#include <string>

namespace nx::cli {

struct BatchParseResult {
    CliErrorCode error_code;
    std::string message;
    bool success;
    
    static BatchParseResult ok() {
        return {CliErrorCode::NONE, "", true};
    }
    
    static BatchParseResult error(CliErrorCode code, const std::string& msg) {
        return {code, msg, false};
    }
};

class BatchArgumentParser {
public:
    /**
     * Parse arguments for batch run operation
     * Requires: --file <path>
     * Optional: --dry-run, --json
     */
    static BatchParseResult parse_run_args(const std::vector<std::string>& args, BatchRunRequest& request);
    
    /**
     * Parse arguments for batch validate operation
     * Requires: --file <path>
     * Optional: --json
     */
    static BatchParseResult parse_validate_args(const std::vector<std::string>& args, BatchValidateRequest& request);
    
    /**
     * Parse arguments for batch summarize operation
     * Requires: --file <path>
     * Optional: --json
     */
    static BatchParseResult parse_summary_args(const std::vector<std::string>& args, BatchSummaryRequest& request);

private:
    static bool has_flag(const std::vector<std::string>& args, const std::string& flag);
    static std::string get_flag_value(const std::vector<std::string>& args, const std::string& flag);
    static BatchParseResult validate_no_unknown_flags(const std::vector<std::string>& args, 
                                                      const std::vector<std::string>& allowed_flags);
    static BatchParseResult validate_no_duplicates(const std::vector<std::string>& args);
};

} // namespace nx::cli