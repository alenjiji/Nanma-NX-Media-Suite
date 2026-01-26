#pragma once

#include <string>
#include <vector>
#include <optional>

namespace nx::batch {

enum class JobState { 
    Queued, 
    Planned, 
    Rejected 
};

struct JobId {
    std::string value;
    
    bool operator==(const JobId& other) const = default;
};

struct ParsedBatchCommand {
    std::string command;
    std::vector<std::string> arguments;
    bool valid;
    
    bool operator==(const ParsedBatchCommand& other) const = default;
};

struct BatchJobSummary {
    JobId job_id;
    std::string command;
    JobState state;
    
    bool operator==(const BatchJobSummary& other) const = default;
};

struct BatchJobDetail {
    JobId job_id;
    std::string command;
    JobState state;
    std::vector<std::string> arguments;
    
    bool operator==(const BatchJobDetail& other) const = default;
};

struct BatchPlan {
    std::vector<BatchJobSummary> jobs;
    
    bool operator==(const BatchPlan& other) const = default;
};

} // namespace nx::batch