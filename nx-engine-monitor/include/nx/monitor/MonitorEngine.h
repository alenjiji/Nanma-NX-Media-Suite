#pragma once

#include <string>
#include <vector>
#include <optional>

namespace nx::monitor {

struct EngineVersion {
    std::string name;
    std::string version;
    std::string build_id;
};

struct EngineInfo {
    std::string name;
    std::string version;
    bool available;
};

struct JobSummary {
    std::string job_id;
    std::string engine;
    std::string state;   // "queued", "running", "completed", "failed"
};

struct JobDetail {
    std::string job_id;
    std::string engine;
    std::string state;
    std::string created_at;
    std::optional<std::string> completed_at;
};

struct SystemStatus {
    bool healthy;
    size_t active_jobs;
    size_t completed_jobs;
};

class MonitorEngine {
public:
    virtual ~MonitorEngine() = default;

    virtual SystemStatus status() const = 0;
    virtual std::vector<JobSummary> jobs() const = 0;
    virtual std::optional<JobDetail> job(const std::string& job_id) const = 0;
    virtual std::vector<EngineInfo> engines() const = 0;
    virtual EngineVersion version() const = 0;
};

} // namespace nx::monitor