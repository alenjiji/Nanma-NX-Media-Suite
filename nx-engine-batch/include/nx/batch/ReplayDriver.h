#pragma once

#include "ExecutionPersistence.h"
#include "RetryEngine.h"
#include <vector>
#include <unordered_map>

namespace nx::batch {

/** Replay mismatch information for determinism verification
 * 
 * PHASE 10.3 COMPLIANCE:
 * - Reports specific divergence between original and replayed execution
 * - Contains only deterministic comparison data
 * - No runtime state, no environment dependencies
 */
struct ReplayMismatch {
    uint32_t retry_index;                   // OWNED: Retry index where divergence occurred
    ExecutionOutcome expected;              // OWNED: Original persisted outcome
    ExecutionOutcome actual;                // OWNED: Replayed execution outcome
    
    bool operator==(const ReplayMismatch& other) const = default;
};

/**
 * Deterministic replay verification report
 * 
 * PHASE 10.3 RESPONSIBILITY:
 * - Binary verification result: match or divergence
 * - Detailed mismatch information for forensic analysis
 * - Deterministic, reproducible reporting
 * - No influence on future execution decisions
 */
struct ReplayReport {
    bool deterministic_match;               // OWNED: True if replay matched original exactly
    std::vector<ReplayMismatch> mismatches; // OWNED: All detected divergences
    
    /**
     * Create successful replay report (full match)
     * 
     * @return ReplayReport indicating deterministic match
     */
    static ReplayReport success() {
        return ReplayReport{true, {}};
    }
    
    /**
     * Create failed replay report with mismatches
     * 
     * @param mismatches Vector of detected divergences
     * @return ReplayReport indicating deterministic divergence
     */
    static ReplayReport divergence(std::vector<ReplayMismatch> mismatches) {
        return ReplayReport{false, std::move(mismatches)};
    }
    
    bool operator==(const ReplayReport& other) const = default;
};

/**
 * Deterministic replay driver for offline verification
 * 
 * PHASE 10.3 RESPONSIBILITY:
 * - Replays persisted execution records deterministically
 * - Verifies structural equivalence between original and replayed execution
 * - Reports divergences for forensic analysis
 * - Operates completely offline, no runtime integration
 * 
 * ARCHITECTURAL CONSTRAINTS:
 * - Read-only operation, no persistence writes
 * - No influence on live execution behavior
 * - No resume, recovery, or continuation capabilities
 * - No scheduling or retry decision logic
 * - No engine mutation or runtime state consultation
 * 
 * FORBIDDEN RESPONSIBILITIES:
 * - Influencing live execution decisions
 * - Deciding whether to retry based on past outcomes
 * - Skipping execution based on persisted results
 * - Resuming partial execution state
 * - Modifying persisted records or execution order
 * - Integration with runtime execution engines
 */
class ReplayDriver {
public:
    /**
     * Construct replay driver with retry executor for verification
     * 
     * OFFLINE OPERATION:
     * - Executor used only for replay verification, not live execution
     * - No persistence recorder attached to prevent writes
     * - Complete isolation from runtime execution pipeline
     * 
     * @param retry_executor Retry executor for replaying execution attempts
     */
    explicit ReplayDriver(std::shared_ptr<RetryExecutor> retry_executor);
    
    /**
     * Replay and verify persisted execution records
     * 
     * REPLAY CONTRACT:
     * - Loads all execution records from replay source
     * - Reconstructs deterministic execution order from persisted data
     * - Re-executes each attempt with fresh SessionJobId
     * - Compares replayed outcomes against persisted outcomes
     * - Reports deterministic match or specific divergences
     * 
     * VERIFICATION PROCESS:
     * 1. Load persisted execution records
     * 2. Reconstruct retry chains and execution order
     * 3. Execute each attempt with identical intent but fresh identity
     * 4. Compare outcomes: intent hashes, retry indices, outcome kinds
     * 5. Generate verification report with match status and divergences
     * 
     * DETERMINISM GUARANTEES:
     * - Replay order matches original logical order
     * - All replayed intents are bit-identical to originals
     * - Outcome comparison is structural, not incidental
     * - No short-circuiting based on persisted results
     * 
     * @param source Execution replay source containing persisted records
     * @return ReplayReport indicating deterministic match or divergences
     */
    ReplayReport replay_and_verify(const ExecutionReplaySource& source);
    
private:
    std::shared_ptr<RetryExecutor> retry_executor_;  // REFERENCED: Executor for replay verification
    
    // Reconstruct retry chains from persisted execution records
    // NOTE: Currently preparatory - reconstructed chains are not consumed in verification
    // This provides foundation for future retry chain validation if needed
    std::vector<RetryChain> reconstruct_retry_chains(const std::vector<ExecutionRecord>& records);
    
    // Verify single execution attempt against persisted record
    std::optional<ReplayMismatch> verify_attempt(
        const ExecutionRecord& original_record,
        const JobExecutionResult& replay_result
    );
    
    // Generate fresh session ID for replay execution
    // NOTE: Returns constant "replay-session" for deterministic replay behavior
    SessionId generate_replay_session_id() const;
};

/**
 * Controllable retry executor for replay testing
 * 
 * TESTING SUPPORT:
 * - Allows forced outcomes for divergence testing
 * - Maintains deterministic behavior for verification
 * - No persistence writes during replay
 */
class TestableReplayExecutor : public RetryExecutor {
public:
    /**
     * Construct testable replay executor
     * 
     * @param recorder Optional recorder (should be nullptr for replay)
     */
    explicit TestableReplayExecutor(ExecutionRecorder* recorder = nullptr);
    
    /**
     * Force specific outcome for testing divergence detection
     * 
     * @param retry_index Retry index to force outcome for
     * @param forced_outcome Outcome to return for specified retry index
     */
    void force_outcome_for_retry_index(uint32_t retry_index, ExecutionOutcome forced_outcome);
    
    /**
     * Execute retry with potential forced outcome for testing
     * 
     * @param intent Job specification to execute
     * @param attempt Retry attempt identity
     * @return Job execution result (potentially forced for testing)
     */
    JobExecutionResult execute_retry(
        const JobExecutionSpec& intent,
        const RetryAttempt& attempt
    ) override;
    
private:
    std::unordered_map<uint32_t, ExecutionOutcome> forced_outcomes_;  // OWNED: Forced outcomes for testing
    
    // Convert ExecutionOutcome to JobExecutionResult for testing
    JobExecutionResult outcome_to_result(const ExecutionOutcome& outcome, const RetryAttempt& attempt) const;
};

} // namespace nx::batch