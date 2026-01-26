#pragma once

#include <string>

namespace nx::cli {

/**
 * Request for global system status snapshot
 */
struct MonitorStatusRequest {
    struct Flags {
        bool json_output = false;
    } flags;
};

/**
 * Request for jobs list snapshot
 */
struct MonitorJobsRequest {
    struct Flags {
        bool json_output = false;
    } flags;
};

/**
 * Request for single job snapshot
 */
struct MonitorJobRequest {
    std::string job_id;
    struct Flags {
        bool json_output = false;
    } flags;
};

/**
 * Request for engines list snapshot
 */
struct MonitorEnginesRequest {
    struct Flags {
        bool json_output = false;
    } flags;
};

/**
 * Request for version information
 */
struct MonitorVersionRequest {
    struct Flags {
        bool json_output = false;
    } flags;
};

} // namespace nx::cli