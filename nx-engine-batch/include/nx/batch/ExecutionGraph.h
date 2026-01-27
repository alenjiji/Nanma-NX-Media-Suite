#pragma once

#include "SessionTypes.h"
#include "JobExecutionSpec.h"
#include <vector>
#include <optional>
#include <unordered_map>
#include <functional>

namespace nx::batch {

/**
 * Execution node bridging intent and execution identities
 * 
 * ARCHITECTURAL RESPONSIBILITY:
 * - Bridges immutable JobExecutionSpec (intent) with ephemeral SessionJobId (execution)
 * - Maintains mapping between the two identity types
 * - Enables scheduler to reason about intent, executor to track execution
 */
struct ExecutionNode {
    SessionJobId job_id;                        // EPHEMERAL: execution identity
    JobExecutionSpec spec;                      // IMMUTABLE: intent identity
    std::vector<SessionJobId> dependencies;     // COPIED: dependency structure from session
    
    bool operator==(const ExecutionNode& other) const = default;
};

/**
 * Execution graph with intent-execution identity bridge
 * 
 * PHASE 9 COMPLIANCE:
 * - Maintains strict separation between JobExecutionSpec and SessionJobId
 * - Enables mapping from execution identity to intent identity
 * - Supports deterministic execution while preserving intent immutability
 */
class ExecutionGraph {
public:
    explicit ExecutionGraph(std::vector<ExecutionNode> nodes);
    
    const std::vector<ExecutionNode>& nodes() const;
    std::optional<ExecutionNode> node(const SessionJobId& job_id) const;
    
    /**
     * Get JobExecutionSpec for given SessionJobId
     * 
     * PHASE 9 BRIDGE:
     * - Maps from ephemeral execution identity to immutable intent
     * - Enables executor to access job intent without owning it
     * 
     * @param job_id Execution identity
     * @return JobExecutionSpec for the job, or nullopt if not found
     */
    std::optional<JobExecutionSpec> get_spec(const SessionJobId& job_id) const;
    
    /**
     * Get all JobExecutionSpecs in deterministic order
     * 
     * @return Vector of all job specifications
     */
    std::vector<JobExecutionSpec> get_all_specs() const;
    
    size_t node_count() const;
    
    bool operator==(const ExecutionGraph& other) const = default;

private:
    std::vector<ExecutionNode> nodes_;                                      // OWNED: All execution nodes
    std::unordered_map<SessionJobId, size_t> job_id_to_index_;             // OWNED: Fast lookup mapping
    
    // Build lookup index for fast job_id queries
    void build_lookup_index();
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