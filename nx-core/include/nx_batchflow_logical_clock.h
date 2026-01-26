#pragma once

#include "nx_batchflow_jobid.h"
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace nx::batchflow {

/// LogicalTick represents a monotonic, deterministic time unit
/// Advances only on explicit BatchFlow events, never on wall-clock time
using LogicalTick = uint64_t;

/// RetryReason represents constrained reasons for retry decisions
/// Prevents semantic leakage through unconstrained string data
enum class RetryReason {
    PolicyAllowed,    // Retry policy permits another attempt
    PolicyDenied      // Retry policy denies further attempts
};

/// FailureCategory represents constrained failure classifications
/// Prevents semantic leakage through unconstrained failure descriptions
enum class FailureCategory {
    EngineError,      // Engine reported an error
    ValidationFailed, // Job validation failed
    DependencyFailed  // Job dependency was not satisfied
};

/// EventData represents structured, constrained event information
/// Replaces unconstrained string details to prevent semantic leakage
struct EventData {
    /// Event data type enumeration
    enum class Type { None, Retry, Failure };
    
    /// Default constructor for events with no additional data
    EventData() : type_(Type::None) {}
    
    /// Constructor for retry decision events
    explicit EventData(RetryReason reason) : type_(Type::Retry), retry_reason_(reason) {}
    
    /// Constructor for failure events
    explicit EventData(FailureCategory category) : type_(Type::Failure), failure_category_(category) {}
    
    /// Get event data type
    Type type() const { return type_; }
    
    /// Get retry reason (only valid if type is Retry)
    RetryReason retry_reason() const { return retry_reason_; }
    
    /// Get failure category (only valid if type is Failure)
    FailureCategory failure_category() const { return failure_category_; }
    
    /// Equality for deterministic comparison
    bool operator==(const EventData& other) const {
        if (type_ != other.type_) return false;
        switch (type_) {
            case Type::None: return true;
            case Type::Retry: return retry_reason_ == other.retry_reason_;
            case Type::Failure: return failure_category_ == other.failure_category_;
        }
        return false;
    }
    
    /// String representation for serialization
    std::string to_string() const;

private:
    Type type_;
    union {
        RetryReason retry_reason_;
        FailureCategory failure_category_;
    };
};

/// BatchFlowEvent represents deterministic events that advance the LogicalClock
/// Only these events are allowed to increment logical time
enum class BatchFlowEvent {
    JobStarted,      // Job execution begins
    JobCompleted,    // Job execution succeeds
    JobFailed,       // Job execution fails
    RetryDecision    // Retry policy decision made
};

/// EventRecord captures a single deterministic event for replay
/// Contains all information needed to reconstruct logical time progression
struct EventRecord {
    LogicalTick tick;           // When this event occurred in logical time
    BatchFlowEvent event_type;  // What type of event occurred
    JobId job_id;              // Which job this event relates to
    EventData data;            // Structured event data (no unconstrained strings)
    
    /// Create event record with no additional data
    EventRecord(LogicalTick t, BatchFlowEvent event, JobId job)
        : tick(t), event_type(event), job_id(std::move(job)), data() {}
    
    /// Create event record with structured data
    EventRecord(LogicalTick t, BatchFlowEvent event, JobId job, EventData event_data)
        : tick(t), event_type(event), job_id(std::move(job)), data(std::move(event_data)) {}
    
    /// String representation for serialization
    std::string to_string() const;
    
    /// Equality for deterministic comparison
    bool operator==(const EventRecord& other) const {
        return tick == other.tick && 
               event_type == other.event_type && 
               job_id == other.job_id && 
               data == other.data;
    }
};

/// LogicalClock provides deterministic, monotonic time for BatchFlow orchestration
/// Time advances ONLY on explicit BatchFlow events, never on wall-clock time
/// Completely replayable from event logs for deterministic behavior
class LogicalClock {
public:
    /// Create logical clock starting at tick 0
    LogicalClock() : current_tick_(0) {}
    
    /// Get current logical time
    LogicalTick current_tick() const noexcept { return current_tick_; }
    
    /// Advance clock on job start event
    /// Returns the tick when this job started
    LogicalTick on_job_started(const JobId& job_id);
    
    /// Advance clock on job completion event
    /// Returns the tick when this job completed
    LogicalTick on_job_completed(const JobId& job_id);
    
    /// Advance clock on job failure event
    /// Returns the tick when this job failed
    LogicalTick on_job_failed(const JobId& job_id, FailureCategory category = FailureCategory::EngineError);
    
    /// Advance clock on retry decision event
    /// Returns the tick when retry decision was made
    LogicalTick on_retry_decision(const JobId& job_id, RetryReason reason);
    
    /// Get complete event history for replay
    /// Events are ordered by logical tick (deterministic ordering)
    const std::vector<EventRecord>& event_history() const { return event_history_; }
    
    /// Get events for a specific job (ordered by tick)
    std::vector<EventRecord> get_job_events(const JobId& job_id) const;
    
    /// Replay clock from event history
    /// Reconstructs logical clock state by re-deriving ticks from events
    /// Validates that regenerated ticks match recorded ticks (determinism check)
    /// Used for deterministic replay of BatchFlow execution
    static LogicalClock replay_from_events(const std::vector<EventRecord>& events);
    
    /// Reset clock to initial state (tick 0, no events)
    void reset();
    
    /// Get total number of events recorded
    size_t event_count() const noexcept { return event_history_.size(); }
    
    /// Check if clock has recorded any events
    bool has_events() const noexcept { return !event_history_.empty(); }
    
    /// String representation for serialization
    std::string to_string() const;

private:
    LogicalTick current_tick_;                    // Current logical time
    std::vector<EventRecord> event_history_;     // Complete event log for replay
    std::map<JobId, std::vector<LogicalTick>> job_event_ticks_;  // Job-specific event tracking
    
    /// Internal method to advance clock and record event
    LogicalTick advance_and_record(BatchFlowEvent event_type, const JobId& job_id, const EventData& data = EventData());
};

/// Event type to string conversion for serialization
inline std::string event_type_to_string(BatchFlowEvent event) {
    switch (event) {
        case BatchFlowEvent::JobStarted:    return "JobStarted";
        case BatchFlowEvent::JobCompleted:  return "JobCompleted";
        case BatchFlowEvent::JobFailed:     return "JobFailed";
        case BatchFlowEvent::RetryDecision: return "RetryDecision";
        default: return "Unknown";
    }
}

/// String to event type conversion for deserialization
inline BatchFlowEvent string_to_event_type(const std::string& str) {
    if (str == "JobStarted") return BatchFlowEvent::JobStarted;
    if (str == "JobCompleted") return BatchFlowEvent::JobCompleted;
    if (str == "JobFailed") return BatchFlowEvent::JobFailed;
    if (str == "RetryDecision") return BatchFlowEvent::RetryDecision;
    throw std::invalid_argument("Unknown event type: " + str);
}

/// Implementation of LogicalClock methods
inline LogicalTick LogicalClock::on_job_started(const JobId& job_id) {
    return advance_and_record(BatchFlowEvent::JobStarted, job_id);
}

inline LogicalTick LogicalClock::on_job_completed(const JobId& job_id) {
    return advance_and_record(BatchFlowEvent::JobCompleted, job_id);
}

inline LogicalTick LogicalClock::on_job_failed(const JobId& job_id, FailureCategory category) {
    return advance_and_record(BatchFlowEvent::JobFailed, job_id, EventData(category));
}

inline LogicalTick LogicalClock::on_retry_decision(const JobId& job_id, RetryReason reason) {
    return advance_and_record(BatchFlowEvent::RetryDecision, job_id, EventData(reason));
}

inline LogicalTick LogicalClock::advance_and_record(BatchFlowEvent event_type, const JobId& job_id, const EventData& data) {
    // Advance logical time (monotonic increment)
    ++current_tick_;
    
    // Record event for replay
    event_history_.emplace_back(current_tick_, event_type, job_id, data);
    
    // Track job-specific events for lookup
    job_event_ticks_[job_id].push_back(current_tick_);
    
    return current_tick_;
}

inline std::vector<EventRecord> LogicalClock::get_job_events(const JobId& job_id) const {
    std::vector<EventRecord> job_events;
    
    // Find all events for this job (events are already ordered by tick)
    for (const auto& event : event_history_) {
        if (event.job_id == job_id) {
            job_events.push_back(event);
        }
    }
    
    return job_events;
}

inline LogicalClock LogicalClock::replay_from_events(const std::vector<EventRecord>& events) {
    LogicalClock replayed_clock;
    
    // Replay events by re-deriving ticks (determinism validation)
    for (const auto& event : events) {
        // Re-derive tick by advancing clock
        ++replayed_clock.current_tick_;
        
        // Validate that regenerated tick matches recorded tick
        if (replayed_clock.current_tick_ != event.tick) {
            throw std::runtime_error("Replay determinism violation: regenerated tick " + 
                                   std::to_string(replayed_clock.current_tick_) + 
                                   " does not match recorded tick " + 
                                   std::to_string(event.tick));
        }
        
        // Record event in history
        replayed_clock.event_history_.push_back(event);
        
        // Update job event tracking
        replayed_clock.job_event_ticks_[event.job_id].push_back(event.tick);
    }
    
    return replayed_clock;
}

inline void LogicalClock::reset() {
    current_tick_ = 0;
    event_history_.clear();
    job_event_ticks_.clear();
}

inline std::string LogicalClock::to_string() const {
    std::ostringstream result;
    result << "LogicalClock{tick=" << current_tick_ << ", events=" << event_history_.size() << "}";
    return result.str();
}

inline std::string EventRecord::to_string() const {
    std::ostringstream result;
    result << "Event{tick=" << tick 
           << ", type=" << event_type_to_string(event_type)
           << ", job=" << job_id.to_string()
           << ", data=" << data.to_string() << "}";
    return result.str();
}

inline std::string EventData::to_string() const {
    switch (type_) {
        case Type::None: return "None";
        case Type::Retry: 
            return std::string("Retry:") + (retry_reason_ == RetryReason::PolicyAllowed ? "PolicyAllowed" : "PolicyDenied");
        case Type::Failure:
            switch (failure_category_) {
                case FailureCategory::EngineError: return "Failure:EngineError";
                case FailureCategory::ValidationFailed: return "Failure:ValidationFailed";
                case FailureCategory::DependencyFailed: return "Failure:DependencyFailed";
            }
    }
    return "Unknown";
}

} // namespace nx::batchflow