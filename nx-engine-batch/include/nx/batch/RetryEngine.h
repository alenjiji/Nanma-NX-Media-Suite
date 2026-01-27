#pragma once

#include "SessionTypes.h"
#include "JobExecutionSpec.h"
#include "JobExecutionResult.h"
#include "ExecutionPersistence.h"
#include <vector>
#include <optional>

namespace nx::batch {

/**
 * Retry attempt identity for deterministic retry lineage
 * 
 * PHASE 10.1 COMPLIANCE:
 * - Each retry attempt has unique SessionJobId
 * - Parent-child relationships establish retry lineage
 * - Retry index provides deterministic ordering
 * - No mutation of original intent
 */
struct RetryAttempt {
    SessionJobId attempt_id;                            // OWNED: Unique execution attempt identity
    std::optional<SessionJobId> parent_attempt_id;      // REFERENCED: Parent attempt for lineage
    uint32_t retry_index;                               // OWNED: Retry attempt number (0 = original)
    
    /**
     * Create initial retry attempt (original execution)
     * 
     * @param session_id Session identity
     * @param job_value Job identifier within session
     * @return RetryAttempt with retry_index = 0, no parent
     */
    static RetryAttempt create_initial(SessionId session_id, std::string job_value);
    
    /**
     * Create retry attempt from parent
     * 
     * @param parent_attempt Previous attempt in retry chain
     * @return RetryAttempt with incremented retry_index and parent linkage
     */
    static RetryAttempt create_retry(const RetryAttempt& parent_attempt);
    
    bool operator==(const RetryAttempt& other) const = default;
};

/**
 * Retry chain for deterministic retry execution
 * 
 * PHASE 10.1 RESPONSIBILITY:
 * - Maintains immutable intent across all retry attempts
 * - Tracks retry lineage with parent-child relationships
 * - Provides deterministic retry ordering
 * - Enables replay of complete retry sequence
 * 
 * ARCHITECTURAL CONSTRAINTS:
 * - Intent (JobExecutionSpec) is immutable across all attempts
 * - Each attempt has unique SessionJobId
 * - Retry order is explicit and deterministic
 * - No autonomous retry decisions
 */
struct RetryChain {
    JobExecutionSpec intent;                            // OWNED: Immutable intent for all attempts
    std::vector<RetryAttempt> attempts;                 // OWNED: All retry attempts in order
    
    /**
     * Create retry chain with initial attempt
     * 
     * @param intent Immutable job specification for all attempts
     * @param initial_attempt First attempt in retry chain
     * @return RetryChain with single initial attempt
     */
    static RetryChain create(JobExecutionSpec intent, RetryAttempt initial_attempt);
    
    /**
     * Add retry attempt to chain
     * 
     * DETERMINISTIC ORDERING:
     * - New attempt is appended to attempts vector
     * - Parent-child relationship is established
     * - Retry index is incremented deterministically
     * 
     * @param parent_attempt Previous attempt to retry from
     * @return New RetryAttempt added to chain
     */
    RetryAttempt add_retry(const RetryAttempt& parent_attempt);
    
    /**
     * Get current (most recent) attempt
     * 
     * @return Most recent retry attempt in chain
     */
    const RetryAttempt& current_attempt() const;
    
    /**
     * Get total number of attempts (including original)
     * 
     * @return Total attempt count
     */
    size_t attempt_count() const;
    
    bool operator==(const RetryChain& other) const = default;
};

/**
 * Deterministic retry executor for explicit retry execution
 * 
 * PHASE 10.1 RESPONSIBILITY:
 * - Executes retry attempts with identical intent
 * - Maintains retry lineage and deterministic ordering
 * - Records execution attempts for replay
 * - Provides zero autonomy - only executes when explicitly instructed
 * 
 * FORBIDDEN RESPONSIBILITIES:
 * - Deciding whether to retry (no autonomous behavior)
 * - Mutating JobExecutionSpec between attempts
 * - Reusing SessionJobId across attempts
 * - Implementing retry policies or backoff logic
 */
class RetryExecutor {
public:
    /**
     * Construct retry executor with persistence recorder
     * 
     * @param recorder Execution recorder for persistence (may be nullptr)
     */
    explicit RetryExecutor(ExecutionRecorder* recorder = nullptr);
    
    /**
     * Execute retry attempt with deterministic behavior
     * 
     * EXECUTION CONTRACT:
     * - Intent (JobExecutionSpec) is executed without mutation
     * - Attempt identity (SessionJobId) is unique for this execution
     * - Execution is completely isolated from previous attempts
     * - Result is recorded for replay if recorder is provided
     * 
     * @param intent Immutable job specification to execute
     * @param attempt Retry attempt identity and lineage
     * @return Execution result for this specific attempt
     */
    JobExecutionResult execute_retry(
        const JobExecutionSpec& intent,
        const RetryAttempt& attempt
    );
    
private:
    ExecutionRecorder* recorder_;  // REFERENCED: Optional execution recorder for persistence
    
    // Convert JobExecutionResult to ExecutionOutcome for persistence
    ExecutionOutcome result_to_outcome(const JobExecutionResult& result) const;
};

} // namespace nx::batch