#pragma once

#include "cli_types.h"
#include "monitor_types.h"
#include <vector>
#include <string>

namespace nx::cli {

class MonitorCommand {
public:
    /**
     * Main entry point for monitor component
     */
    static CliResult execute(const std::vector<std::string>& args);
    
    /**
     * Execute monitor status operation - global system snapshot
     */
    static CliResult handle_status(const MonitorStatusRequest& request);
    
    /**
     * Execute monitor jobs operation - jobs list snapshot
     */
    static CliResult handle_jobs(const MonitorJobsRequest& request);
    
    /**
     * Execute monitor job operation - single job snapshot
     */
    static CliResult handle_job(const MonitorJobRequest& request);
    
    /**
     * Execute monitor engines operation - engines list snapshot
     */
    static CliResult handle_engines(const MonitorEnginesRequest& request);
    
    /**
     * Execute monitor version operation - version information
     */
    static CliResult handle_version(const MonitorVersionRequest& request);
};

} // namespace nx::cli