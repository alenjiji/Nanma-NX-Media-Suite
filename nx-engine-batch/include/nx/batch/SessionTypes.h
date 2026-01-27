#pragma once

#include <string>

namespace nx::batch {

struct SessionId {
    std::string value;
    
    bool operator==(const SessionId& other) const = default;
    bool operator<(const SessionId& other) const { return value < other.value; }
};

struct SessionJobId {
    SessionId session;
    std::string job_value;
    
    bool operator==(const SessionJobId& other) const = default;
    bool operator<(const SessionJobId& other) const {
        if (session.value != other.session.value) {
            return session.value < other.session.value;
        }
        return job_value < other.job_value;
    }
};

} // namespace nx::batch