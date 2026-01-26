#pragma once

#include "nx_batchflow_dag.h"
#include "nx_batchflow_logical_clock.h"
#include <map>
#include <vector>
#include <algorithm>

namespace nx::batchflow {

/// JobState represents the execution state of a job in the scheduler
/// Clear state transitions: Pending → Running → (Completed | Failed)
enum class JobState {
    Pending,    // Job waiting for dependencies to complete
    Running,    // Job currently executing
    Completed,  // Job finished successfully
    Failed      // Job finished with failure
};

/// JobStatus contains current state and execution information for a job
struct JobStatus {
    JobState state;
    LogicalTick started_tick;    // When job started (0 if not started)
    LogicalTick finished_tick;   // When job finished (0 if not finished)
    
    JobStatus() : state(JobState::Pending), started_tick(0), finished_tick(0) {}
    
    bool is_finished() const { return state == JobState::Completed || state == JobState::Failed; }
    bool is_ready_to_run() const { return state == JobState::Pending; }
    bool is_running() const { return state == JobState::Running; }
    
    /// Equality operator for deterministic comparison
    bool operator==(const JobStatus& other) const noexcept {
        return state == other.state && 
               started_tick == other.started_tick && 
               finished_tick == other.finished_tick;
    }
};

/// BatchFlowScheduler coordinates job state transitions in deterministic order
/// Does NOT execute jobs - only manages readiness and state transitions
/// External executor reports completion/failure back to scheduler
class BatchFlowScheduler {
public:
    /// Create scheduler with immutable DAG and logical clock
    /// DAG must be finalized before passing to scheduler
    BatchFlowScheduler(const JobGraph& dag, LogicalClock& clock);
    
    /// Get next jobs ready to run (deterministic order)
    /// Returns empty vector if no jobs are ready
    std::vector<JobId> next_ready_jobs() const;
    
    /// Start a job (transition Pending → Running)
    /// External executor should call this before beginning job execution
    /// Returns the logical tick when job was started
    LogicalTick start_job(const JobId& job_id);
    
    /// Mark job as completed (transition Running → Completed)
    /// External executor calls this when job finishes successfully
    /// Returns the logical tick when job was completed
    LogicalTick mark_completed(const JobId& job_id);
    
    /// Mark job as failed (transition Running → Failed)
    /// External executor calls this when job fails
    /// Returns the logical tick when job failed
    LogicalTick mark_failed(const JobId& job_id, FailureCategory category);
    
    /// Replay-only methods for clockless state transitions
    /// These methods update JobStatus without advancing LogicalClock
    /// Used during replay when clock is already reconstructed
    
    /// Replay job start (transition Pending → Running) without clock advancement
    void replay_start_job(const JobId& job_id, LogicalTick tick);
    
    /// Replay job completion (transition Running → Completed) without clock advancement
    void replay_mark_completed(const JobId& job_id, LogicalTick tick);
    
    /// Replay job failure (transition Running → Failed) without clock advancement
    void replay_mark_failed(const JobId& job_id, FailureCategory category, LogicalTick tick);
    
    /// Get current status of a specific job
    const JobStatus& get_job_status(const JobId& job_id) const;
    
    /// Get all job statuses (ordered by JobId for determinism)
    std::map<JobId, JobStatus> get_all_statuses() const;
    
    /// Check if scheduler has any running jobs
    bool has_running_jobs() const;
    
    /// Check if all jobs are finished (completed or failed)
    bool all_jobs_finished() const;
    
    /// Get count of jobs in each state
    size_t count_pending() const;
    size_t count_running() const;
    size_t count_completed() const;
    size_t count_failed() const;

private:
    const JobGraph& dag_;           // Immutable reference to job graph
    LogicalClock& clock_;           // Reference to logical clock for events
    std::map<JobId, JobStatus> job_statuses_;  // Current state of all jobs
    
    /// Check if job's dependencies are all completed
    bool are_dependencies_satisfied(const JobId& job_id) const;
    
    /// Deterministic tie-breaking for job ordering
    /// Uses JobId string comparison for reproducible order
    static bool job_id_less_than(const JobId& a, const JobId& b) {
        return a.hash() < b.hash();
    }
};

/// Implementation of BatchFlowScheduler methods
inline BatchFlowScheduler::BatchFlowScheduler(const JobGraph& dag, LogicalClock& clock)
    : dag_(dag), clock_(clock) {
    
    // Initialize all jobs to Pending state
    for (const auto& node : dag_.nodes()) {
        job_statuses_[node.id()] = JobStatus();
    }
}

inline std::vector<JobId> BatchFlowScheduler::next_ready_jobs() const {
    std::vector<JobId> ready_jobs;
    
    // Find all pending jobs with satisfied dependencies
    for (const auto& [job_id, status] : job_statuses_) {
        if (status.is_ready_to_run() && are_dependencies_satisfied(job_id)) {
            ready_jobs.push_back(job_id);
        }
    }
    
    // Sort for deterministic ordering (tie-breaking by JobId)
    std::sort(ready_jobs.begin(), ready_jobs.end(), job_id_less_than);
    
    return ready_jobs;
}

inline LogicalTick BatchFlowScheduler::start_job(const JobId& job_id) {
    auto& status = job_statuses_[job_id];
    if (status.state != JobState::Pending) {
        throw std::invalid_argument("Job is not in Pending state");
    }
    status.state = JobState::Running;
    status.started_tick = clock_.on_job_started(job_id);
    return status.started_tick;
}

inline LogicalTick BatchFlowScheduler::mark_completed(const JobId& job_id) {
    auto& status = job_statuses_[job_id];
    if (status.state != JobState::Running) {
        throw std::invalid_argument("Job is not in Running state");
    }
    status.state = JobState::Completed;
    status.finished_tick = clock_.on_job_completed(job_id);
    return status.finished_tick;
}

inline LogicalTick BatchFlowScheduler::mark_failed(const JobId& job_id, FailureCategory category) {
    auto& status = job_statuses_[job_id];
    if (status.state != JobState::Running) {
        throw std::invalid_argument("Job is not in Running state");
    }
    status.state = JobState::Failed;
    status.finished_tick = clock_.on_job_failed(job_id, category);
    return status.finished_tick;
}

/// Clockless replay methods - update JobStatus without advancing LogicalClock
inline void BatchFlowScheduler::replay_start_job(const JobId& job_id, LogicalTick tick) {
    auto& status = job_statuses_[job_id];
    if (status.state != JobState::Pending) {
        throw std::invalid_argument("Job is not in Pending state");
    }
    status.state = JobState::Running;
    status.started_tick = tick;  // Set tick directly, no clock advancement
}

inline void BatchFlowScheduler::replay_mark_completed(const JobId& job_id, LogicalTick tick) {
    auto& status = job_statuses_[job_id];
    if (status.state != JobState::Running) {
        throw std::invalid_argument("Job is not in Running state");
    }
    status.state = JobState::Completed;
    status.finished_tick = tick;  // Set tick directly, no clock advancement
}

inline void BatchFlowScheduler::replay_mark_failed(const JobId& job_id, FailureCategory category, LogicalTick tick) {
    auto& status = job_statuses_[job_id];
    if (status.state != JobState::Running) {
        throw std::invalid_argument("Job is not in Running state");
    }
    status.state = JobState::Failed;
    status.finished_tick = tick;  // Set tick directly, no clock advancement
}

inline bool BatchFlowScheduler::are_dependencies_satisfied(const JobId& job_id) const {
    std::vector<JobId> dependencies = dag_.get_dependencies(job_id);
    
    // All dependencies must be completed
    for (const JobId& dep_id : dependencies) {
        auto it = job_statuses_.find(dep_id);
        if (it == job_statuses_.end() || it->second.state != JobState::Completed) {
            return false;
        }
    }
    
    return true;
}

inline const JobStatus& BatchFlowScheduler::get_job_status(const JobId& job_id) const {
    auto it = job_statuses_.find(job_id);
    if (it == job_statuses_.end()) {
        throw std::invalid_argument("Job not found in scheduler");
    }
    return it->second;
}

inline std::map<JobId, JobStatus> BatchFlowScheduler::get_all_statuses() const {
    return job_statuses_; // Already ordered by JobId (deterministic)
}

inline bool BatchFlowScheduler::has_running_jobs() const {
    for (const auto& [job_id, status] : job_statuses_) {
        if (status.is_running()) {
            return true;
        }
    }
    return false;
}

inline bool BatchFlowScheduler::all_jobs_finished() const {
    for (const auto& [job_id, status] : job_statuses_) {
        if (!status.is_finished()) {
            return false;
        }
    }
    return true;
}

inline size_t BatchFlowScheduler::count_pending() const {
    size_t count = 0;
    for (const auto& [job_id, status] : job_statuses_) {
        if (status.state == JobState::Pending) count++;
    }
    return count;
}

inline size_t BatchFlowScheduler::count_running() const {
    size_t count = 0;
    for (const auto& [job_id, status] : job_statuses_) {
        if (status.state == JobState::Running) count++;
    }
    return count;
}

inline size_t BatchFlowScheduler::count_completed() const {
    size_t count = 0;
    for (const auto& [job_id, status] : job_statuses_) {
        if (status.state == JobState::Completed) count++;
    }
    return count;
}

inline size_t BatchFlowScheduler::count_failed() const {
    size_t count = 0;
    for (const auto& [job_id, status] : job_statuses_) {
        if (status.state == JobState::Failed) count++;
    }
    return count;
}

} // namespace nx::batchflow