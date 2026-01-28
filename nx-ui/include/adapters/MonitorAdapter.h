// PHASE 15 INVARIANT:
// This adapter is a stateless CLI bridge.
// It adds no defaults, no retries, no inference.
// All parameters map 1:1 to CLI arguments.
// If a value is missing, this is a caller error.

#pragma once

#include <string>

namespace nx::ui::adapters {

struct MonitorStatusDto {};
struct MonitorJobsDto {};
struct MonitorJobDto {};
struct MonitorEnginesDto {};
struct MonitorVersionDto {};

template <typename Dto>
struct CliTypedResult {
    int exit_code;
    std::string raw_json;
};

struct MonitorStatusArgs {
    std::string format;
};

struct MonitorJobsArgs {
    std::string format;
    std::string engine_filter;
};

struct MonitorJobArgs {
    std::string job_id;
    std::string format;
};

struct MonitorEnginesArgs {
    std::string format;
};

struct MonitorVersionArgs {
    std::string format;
};

class MonitorAdapter {
public:
    static CliTypedResult<MonitorStatusDto> status(const MonitorStatusArgs& args);
    static CliTypedResult<MonitorJobsDto> jobs(const MonitorJobsArgs& args);
    static CliTypedResult<MonitorJobDto> job(const MonitorJobArgs& args);
    static CliTypedResult<MonitorEnginesDto> engines(const MonitorEnginesArgs& args);
    static CliTypedResult<MonitorVersionDto> version(const MonitorVersionArgs& args);
};

} // namespace nx::ui::adapters