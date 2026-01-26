#pragma once

#include "nx_batchflow_logical_clock.h"
#include "nx_batchflow_scheduler.h"
#include <set>
#include <cstdint>

namespace nx::batchflow {

/// RetryableState represents job states that can trigger retry evaluation
/// Pure declarative enumeration - no conditional logic
enum class RetryableState {
    Failed          // Job failed and may be eligible for retry
};

/// RetryPolicy is a pure declarative data structure defining retry behavior
/// Contains NO conditional logic, NO adaptive behavior, NO engine inspection
struct RetryPolicy {
    uint32_t max_attempts;                          // Maximum total attempts (including original)
    std::set<RetryableState> retry_on_states;      // Which states trigger retry evaluation
    uint32_t retry_delay_ticks;                     // Fixed delay in LogicalClock ticks
    
    /// Create retry policy with explicit parameters
    /// All behavior is determined by these declarative values
    RetryPolicy(uint32_t max_attempts, 
                std::set<RetryableState> retry_states, 
                uint32_t delay_ticks)
        : max_attempts(max_attempts)
        , retry_on_states(std::move(retry_states))
        , retry_delay_ticks(delay_ticks) {}
    
    /// Create no-retry policy (max_attempts = 1)
    static RetryPolicy no_retry() {
        return RetryPolicy(1, {}, 0);
    }
    
    /// Create simple retry policy for failed jobs
    static RetryPolicy retry_on_failure(uint32_t max_attempts, uint32_t delay_ticks) {
        return RetryPolicy(max_attempts, {RetryableState::Failed}, delay_ticks);
    }
    
    /// Equality for deterministic comparison
    bool operator==(const RetryPolicy& other) const {
        return max_attempts == other.max_attempts &&
               retry_on_states == other.retry_on_states &&
               retry_delay_ticks == other.retry_delay_ticks;
    }
    
    /// String representation for serialization
    std::string to_string() const;
};

/// JobRetryState tracks retry attempts for a specific job
/// Pure state tracking - no decision logic
struct JobRetryState {
    uint32_t attempt_count;         // Current attempt number (1-based)
    LogicalTick last_attempt_tick;  // When last attempt was made
    
    JobRetryState() : attempt_count(0), last_attempt_tick(0) {}
    
    /// Check if this is the first attempt
    bool is_first_attempt() const { return attempt_count == 0; }
    
    /// Get next attempt number
    uint32_t next_attempt_number() const { return attempt_count + 1; }
    
    /// Record new attempt at given tick
    void record_attempt(LogicalTick tick) {
        attempt_count++;
        last_attempt_tick = tick;
    }
};

/// RetryDecision represents the outcome of retry policy evaluation
/// Pure data structure - contains decision and reasoning
struct RetryDecision {
    RetryReason reason;             // Why this decision was made
    bool should_retry;              // Whether job should be retried
    LogicalTick earliest_retry_tick; // When retry can occur (if should_retry)
    
    /// Create retry decision
    RetryDecision(RetryReason r, bool retry, LogicalTick earliest_tick = 0)
        : reason(r), should_retry(retry), earliest_retry_tick(earliest_tick) {}
    
    /// Create "retry allowed" decision
    static RetryDecision allow_retry(LogicalTick earliest_tick) {
        return RetryDecision(RetryReason::PolicyAllowed, true, earliest_tick);
    }
    
    /// Create "retry denied" decision
    static RetryDecision deny_retry() {
        return RetryDecision(RetryReason::PolicyDenied, false, 0);
    }
};

/// RetryPolicyEvaluator makes deterministic retry decisions based ONLY on policy and state
/// Contains policy evaluation logic but NO adaptive behavior, NO engine inspection
class RetryPolicyEvaluator {
public:
    /// Evaluate whether job should be retried based on policy and current state
    /// DETERMINISM PROOF: Decision based ONLY on policy data + job state + current tick
    /// NO adaptive behavior, NO engine inspection
    static RetryDecision evaluate_retry(const RetryPolicy& policy,
                                       const JobRetryState& job_state,
                                       RetryableState current_state,
                                       LogicalTick current_tick);
    
    /// Check if enough time has passed for retry (based on policy delay)
    /// DETERMINISM PROOF: Pure arithmetic comparison
    static bool is_retry_time_reached(const JobRetryState& job_state,
                                     uint32_t delay_ticks,
                                     LogicalTick current_tick);
    
    /// Convert JobState to RetryableState (if applicable)
    /// Pure mapping function
    static bool is_retryable_state(JobState state, RetryableState& retryable_state);
};

/// Implementation of RetryPolicy methods
inline std::string RetryPolicy::to_string() const {
    std::ostringstream result;
    result << "RetryPolicy{max_attempts=" << max_attempts
           << ", retry_delay_ticks=" << retry_delay_ticks
           << ", retry_on_states=[";
    
    bool first = true;
    for (const auto& state : retry_on_states) {
        if (!first) result << ",";
        result << (state == RetryableState::Failed ? "Failed" : "Unknown");
        first = false;
    }
    result << "]}";
    
    return result.str();
}

/// Implementation of RetryPolicyEvaluator methods
inline RetryDecision RetryPolicyEvaluator::evaluate_retry(const RetryPolicy& policy,
                                                         const JobRetryState& job_state,
                                                         RetryableState current_state,
                                                         LogicalTick current_tick) {
    // DETERMINISM PROOF: All decisions based on declarative policy data
    
    // Check if current state is eligible for retry according to policy
    if (policy.retry_on_states.find(current_state) == policy.retry_on_states.end()) {
        return RetryDecision::deny_retry();
    }
    
    // Check if max attempts would be exceeded
    if (job_state.next_attempt_number() > policy.max_attempts) {
        return RetryDecision::deny_retry();
    }
    
    // Calculate earliest retry time based on policy delay
    LogicalTick earliest_retry = job_state.last_attempt_tick + policy.retry_delay_ticks;
    
    return RetryDecision::allow_retry(earliest_retry);
}

inline bool RetryPolicyEvaluator::is_retry_time_reached(const JobRetryState& job_state,
                                                       uint32_t delay_ticks,
                                                       LogicalTick current_tick) {
    // DETERMINISM PROOF: Pure arithmetic comparison
    LogicalTick earliest_retry = job_state.last_attempt_tick + delay_ticks;
    return current_tick >= earliest_retry;
}

inline bool RetryPolicyEvaluator::is_retryable_state(JobState state, RetryableState& retryable_state) {
    // Pure mapping function
    switch (state) {
        case JobState::Failed:
            retryable_state = RetryableState::Failed;
            return true;
        case JobState::Pending:
        case JobState::Running:
        case JobState::Completed:
            return false;
    }
    return false;
}

} // namespace nx::batchflow