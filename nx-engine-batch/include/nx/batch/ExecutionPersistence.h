#pragma once

#include "SessionTypes.h"
#include "JobExecutionSpec.h"
#include "JobExecutionResult.h"
#include <vector>
#include <optional>

namespace nx::batch {

/**
 * Deterministic error codes for execution outcomes
 * 
 * PHASE 10.2 COMPLIANCE:
 * - Stable, deterministic error classification
 * - No environment-dependent error information
 * - Sufficient for replay verification and debugging
 */
enum class DeterministicErrorCode {
    None = 0,           // No error (success case)
    InvalidInput,       // Input validation failed
    ProcessingFailed,   // Core processing error
    ResourceUnavailable // Required resource not available
};

/**
 * Deterministic execution outcome for persistence
 * 
 * PHASE 10.2 COMPLIANCE:
 * - Contains deterministic outcome classification
 * - Includes stable error codes for failed executions
 * - No timestamps, no runtime state, no environment data
 * - Sufficient for replay verification, insufficient for resume
 */
struct ExecutionOutcome {
    enum class Kind {
        Success,    // Job completed successfully
        Failed      // Job failed with deterministic error
    };
    
    Kind kind;                              // OWNED: Success or failure classification
    DeterministicErrorCode error_code;      // OWNED: Deterministic error code (None for success)
    
    /**
     * Create successful execution outcome
     * 
     * @return ExecutionOutcome with Success kind and no error
     */
    static ExecutionOutcome success() {
        return ExecutionOutcome{Kind::Success, DeterministicErrorCode::None};
    }
    
    /**
     * Create failed execution outcome with error code
     * 
     * @param error_code Deterministic error classification
     * @return ExecutionOutcome with Failed kind and specified error
     */
    static ExecutionOutcome failed(DeterministicErrorCode error_code) {
        return ExecutionOutcome{Kind::Failed, error_code};
    }
    
    bool operator==(const ExecutionOutcome& other) const = default;
};

/**
 * Immutable execution record for replay-only persistence
 * 
 * ARCHITECTURAL CONSTRAINTS:
 * - Emitted exactly once per execution attempt, after completion
 * - Contains complete intent (JobExecutionSpec) embedded verbatim
 * - Contains retry lineage for deterministic replay
 * - Contains no time fields, no runtime state, no partial results
 * 
 * FORBIDDEN CONTENTS:
 * - Timestamps, wall-clock time, duration measurements
 * - Partial execution state, progress indicators, checkpoints
 * - Environment state, machine configuration, runtime context
 * - Resume pointers, continuation state, recovery information
 */
struct ExecutionRecord {
    SessionJobId attempt_id;                            // OWNED: Unique execution attempt identity
    std::optional<SessionJobId> parent_attempt_id;      // REFERENCED: Parent attempt for retry lineage
    uint32_t retry_index;                               // OWNED: Retry attempt number (0 = original)
    
    JobExecutionSpec intent;                            // OWNED: Complete immutable intent, embedded verbatim
    ExecutionOutcome outcome;                           // OWNED: Deterministic execution outcome
    
    /**
     * Create execution record for completed attempt
     * 
     * PHASE 10.2 CONTRACT:
     * - Called exactly once per execution attempt, after completion
     * - Intent is embedded verbatim for self-sufficient replay
     * - Outcome is deterministic, no environment dependencies
     * 
     * @param attempt_id Unique execution attempt identity
     * @param parent_attempt_id Parent attempt for retry lineage (nullopt for original)
     * @param retry_index Retry attempt number (0 for original attempt)
     * @param intent Complete immutable job specification
     * @param outcome Deterministic execution outcome
     * @return Immutable execution record for persistence
     */
    static ExecutionRecord create(
        SessionJobId attempt_id,
        std::optional<SessionJobId> parent_attempt_id,
        uint32_t retry_index,
        JobExecutionSpec intent,
        ExecutionOutcome outcome
    ) {
        return ExecutionRecord{
            .attempt_id = std::move(attempt_id),
            .parent_attempt_id = std::move(parent_attempt_id),
            .retry_index = retry_index,
            .intent = std::move(intent),
            .outcome = outcome
        };
    }
    
    bool operator==(const ExecutionRecord& other) const = default;
};

/**
 * Write-only execution recorder for runtime persistence
 * 
 * PHASE 10.2 RESPONSIBILITY:
 * - Records completed execution attempts only
 * - Append-only, immutable persistence
 * - No read capability during runtime
 * - No influence on execution decisions
 * 
 * FORBIDDEN RESPONSIBILITIES:
 * - Reading persisted records during execution
 * - Influencing retry decisions based on history
 * - Providing resume or recovery capabilities
 * - Storing partial or in-progress execution state
 */
class ExecutionRecorder {
public:
    virtual ~ExecutionRecorder() = default;
    
    /**
     * Record completed execution attempt
     * 
     * PERSISTENCE CONTRACT:
     * - Called exactly once per execution attempt, after completion
     * - Record is immutable after recording
     * - No runtime state consultation
     * - Append-only operation
     * 
     * @param record Complete execution record for persistence
     */
    virtual void record(const ExecutionRecord& record) = 0;
};

/**
 * Read-only execution replay source for deterministic replay
 * 
 * PHASE 10.2 RESPONSIBILITY:
 * - Provides access to persisted execution records for replay only
 * - No write capability, no runtime state modification
 * - Enables reconstruction of execution order and retry lineage
 * - Supports deterministic replay without resume capability
 * 
 * FORBIDDEN RESPONSIBILITIES:
 * - Modifying persisted records
 * - Providing "latest state" or "current attempt" queries
 * - Enabling resume, rollback, or recovery operations
 * - Consulting records to influence new execution decisions
 */
class ExecutionReplaySource {
public:
    virtual ~ExecutionReplaySource() = default;
    
    /**
     * Load all persisted execution records for replay
     * 
     * REPLAY CONTRACT:
     * - Returns complete execution history in deterministic order
     * - Records are sufficient for full replay reconstruction
     * - Records are insufficient for resume or continuation
     * - No filtering, no "latest state" queries
     * 
     * @return Vector of all execution records in deterministic order
     */
    virtual std::vector<ExecutionRecord> load_all() const = 0;
};

/**
 * In-memory execution recorder for testing and development
 * 
 * DETERMINISTIC BEHAVIOR:
 * - Records stored in append-only vector
 * - Deterministic ordering preserved
 * - No time-based or environment-dependent behavior
 */
class InMemoryExecutionRecorder : public ExecutionRecorder {
public:
    void record(const ExecutionRecord& record) override;
    
    /**
     * Get recorded execution records (testing only)
     * 
     * @return Vector of all recorded execution records
     */
    const std::vector<ExecutionRecord>& get_records() const;
    
    /**
     * Clear all recorded execution records (testing only)
     */
    void clear();
    
private:
    std::vector<ExecutionRecord> records_;  // OWNED: Append-only execution records
};

/**
 * In-memory execution replay source for testing
 * 
 * DETERMINISTIC BEHAVIOR:
 * - Provides deterministic access to execution records
 * - No modification capability during replay
 * - Supports full replay reconstruction
 */
class InMemoryExecutionReplaySource : public ExecutionReplaySource {
public:
    explicit InMemoryExecutionReplaySource(std::vector<ExecutionRecord> records);
    
    std::vector<ExecutionRecord> load_all() const override;
    
private:
    std::vector<ExecutionRecord> records_;  // OWNED: Immutable execution records for replay
};

} // namespace nx::batch