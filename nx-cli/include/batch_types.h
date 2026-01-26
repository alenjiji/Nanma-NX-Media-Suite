#pragma once

#include <string>

namespace nx::cli {

/**
 * Request to execute a batch file sequentially
 */
struct BatchRunRequest {
    std::string batch_file;
    
    struct Flags {
        bool dry_run = false;
        bool json_output = false;
    } flags;
};

/**
 * Request to validate a batch file without execution
 */
struct BatchValidateRequest {
    std::string batch_file;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

/**
 * Request to summarize a batch file contents
 */
struct BatchSummaryRequest {
    std::string batch_file;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

} // namespace nx::cli