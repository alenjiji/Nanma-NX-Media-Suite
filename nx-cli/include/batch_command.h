#pragma once

#include "cli_types.h"
#include "batch_types.h"
#include <vector>
#include <string>

namespace nx::cli {

class BatchCommand {
public:
    /**
     * Main entry point for batch component
     */
    static CliResult execute(const std::vector<std::string>& args);
    
    /**
     * Execute batch run operation - sequential command execution
     */
    static CliResult handle_run(const BatchRunRequest& request);
    
    /**
     * Execute batch validate operation - file validation only
     */
    static CliResult handle_validate(const BatchValidateRequest& request);
    
    /**
     * Execute batch summarize operation - static summary
     */
    static CliResult handle_summarize(const BatchSummaryRequest& request);

private:
    static CliResult read_batch_file(const std::string& file_path, std::vector<std::string>& commands);
    static CliResult validate_command_line(const std::string& line, int line_number);
    static void print_run_output(const BatchRunRequest& request, const std::vector<std::string>& commands);
    static void print_validate_output(const BatchValidateRequest& request, const std::vector<std::string>& commands);
    static void print_summary_output(const BatchSummaryRequest& request, const std::vector<std::string>& commands);
};

} // namespace nx::cli