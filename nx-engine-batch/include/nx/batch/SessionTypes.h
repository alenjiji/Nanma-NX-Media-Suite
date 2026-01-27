#pragma once

#include <string>
#include <cstdint>
#include <chrono>

namespace nx::batch {

struct SessionId {
    std::string value;
    
    bool operator==(const SessionId& other) const = default;
    bool operator<(const SessionId& other) const { return value < other.value; }
};

/**
 * Ephemeral execution identity for one job attempt
 * 
 * LIFECYCLE:
 * - Created only at dispatch time
 * - Destroyed after completion
 * - Never reused for retries
 * 
 * EXECUTION STATE OWNERSHIP:
 * - Contains execution state, attempt index, timestamps
 * - Must NOT modify or own job intent
 * - Represents "this execution attempt" not "what to execute"
 */
struct SessionJobId {
    SessionId session;              // REFERENCED: Session identity
    std::string job_value;          // OWNED: Unique job identifier within session
    uint32_t attempt_index;         // OWNED: Retry attempt number (0 = first attempt)
    
    /**
     * Create initial SessionJobId for first attempt
     * 
     * @param session Session identity
     * @param job_value Unique job identifier
     * @return SessionJobId with attempt_index = 0
     */
    static SessionJobId create_initial(SessionId session, std::string job_value);
    
    /**
     * Create retry SessionJobId for subsequent attempt
     * 
     * @param previous Previous SessionJobId
     * @return New SessionJobId with incremented attempt_index
     */
    static SessionJobId create_retry(const SessionJobId& previous);
    
    bool operator==(const SessionJobId& other) const = default;
    bool operator<(const SessionJobId& other) const {
        if (session.value != other.session.value) {
            return session.value < other.session.value;
        }
        if (job_value != other.job_value) {
            return job_value < other.job_value;
        }
        return attempt_index < other.attempt_index;
    }
};

/**
 * Runtime execution state for SessionJobId
 * 
 * EPHEMERAL NATURE:
 * - Exists only during execution
 * - Tracks runtime state and resources
 * - Destroyed when execution scope ends
 */
enum class RuntimeExecutionState {
    Dispatched,     // Job dispatched for execution
    Running,        // Job currently executing
    Completed,      // Job completed successfully
    Failed,         // Job failed
    Retrying        // Job being retried (transitional state)
};

/**
 * Resource allocation for job execution
 */
struct ResourceAllocation {
    uint32_t cpu_threads = 1;       // CPU threads allocated
    bool gpu_enabled = false;       // GPU acceleration enabled
    uint64_t memory_mb = 512;       // Memory allocation in MB
    
    bool operator==(const ResourceAllocation& other) const = default;
};

/**
 * Runtime session job state (ephemeral)
 * 
 * OWNERSHIP MODEL:
 * - Owned by execution engine during runtime
 * - Contains execution state, not job intent
 * - Destroyed when execution completes
 */
struct RuntimeSessionJob {
    SessionJobId id;                                    // OWNED: Execution identity
    RuntimeExecutionState state;                        // OWNED: Current execution state
    ResourceAllocation resources;                       // OWNED: Allocated resources
    std::chrono::steady_clock::time_point started_at;  // OWNED: Execution start time
    
    bool operator==(const RuntimeSessionJob& other) const = default;
};

} // namespace nx::batch

// Hash specialization for SessionJobId to enable unordered_map usage
namespace std {
    template<>
    struct hash<nx::batch::SessionJobId> {
        size_t operator()(const nx::batch::SessionJobId& job_id) const noexcept {
            size_t h1 = hash<string>{}(job_id.session.value);
            size_t h2 = hash<string>{}(job_id.job_value);
            size_t h3 = hash<uint32_t>{}(job_id.attempt_index);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}