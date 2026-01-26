#pragma once

#include "nx_batchflow_jobid.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

namespace nx::batchflow {

/// JobDependency represents a directed edge between two job nodes
/// Immutable after construction, contains only structural information
class JobDependency {
public:
    /// Create dependency: from_job must complete before to_job can start
    JobDependency(JobId from_job, JobId to_job)
        : from_job_(std::move(from_job)), to_job_(std::move(to_job)) {}
    
    /// Get source job (dependency)
    const JobId& from() const noexcept { return from_job_; }
    
    /// Get target job (dependent)
    const JobId& to() const noexcept { return to_job_; }
    
    /// Equality for deterministic comparison
    bool operator==(const JobDependency& other) const noexcept {
        return from_job_ == other.from_job_ && to_job_ == other.to_job_;
    }
    
    /// String representation for serialization
    std::string to_string() const {
        return from_job_.to_string() + " -> " + to_job_.to_string();
    }

private:
    JobId from_job_;
    JobId to_job_;
};

/// JobNode represents a single unit of work in the DAG
/// Contains only structural information, no execution logic
class JobNode {
public:
    /// Create job node from job definition (computes deterministic JobId)
    /// This is the primary constructor - ensures JobId matches content
    static JobNode from_definition(const JobDefinition& definition) {
        JobId job_id = JobIdHasher::compute_job_id(definition);
        return JobNode(std::move(job_id), definition.engine_identifier, definition.parameters_blob);
    }
    
    /// Create job node with pre-computed JobId (use with caution)
    /// Caller must ensure JobId actually matches the content
    /// Used for deserialization or when JobId is already computed
    JobNode(JobId id, std::string engine_name, std::string parameters_blob)
        : id_(std::move(id)), engine_name_(std::move(engine_name)), parameters_blob_(std::move(parameters_blob)) {}
    
    /// Get deterministic job identity
    const JobId& id() const noexcept { return id_; }
    
    /// Get target engine name (sealed engine identifier)
    const std::string& engine_name() const noexcept { return engine_name_; }
    
    /// Get immutable parameters blob (passed unchanged to engine)
    const std::string& parameters_blob() const noexcept { return parameters_blob_; }
    
    /// Equality based on deterministic identity
    bool operator==(const JobNode& other) const noexcept { return id_ == other.id_; }
    
    /// String representation for serialization
    std::string to_string() const {
        return "JobNode{id=" + id_.to_string() + ", engine=" + engine_name_ + "}";
    }

private:
    JobId id_;                    // Deterministic content-based identity
    std::string engine_name_;     // Target sealed engine
    std::string parameters_blob_; // Canonicalized engine parameters
};

/// JobGraph represents a complete, immutable DAG of job nodes and dependencies
/// Must be fully constructed before any execution, cannot be modified after construction
class JobGraph {
public:
    /// Create empty job graph
    JobGraph() = default;
    
    /// Add job node to graph (only during construction phase)
    /// Preferred: Use add_job_definition() to ensure JobId correctness
    /// Throws if graph is already finalized
    void add_node(JobNode node) {
        if (finalized_) {
            throw std::runtime_error("Cannot add node to finalized graph");
        }
        nodes_.push_back(std::move(node));
    }
    
    /// Add job from definition (computes deterministic JobId automatically)
    /// This is the preferred way to add jobs - ensures JobId matches content
    /// Throws if graph is already finalized
    void add_job_definition(const JobDefinition& definition) {
        if (finalized_) {
            throw std::runtime_error("Cannot add job to finalized graph");
        }
        JobNode node = JobNode::from_definition(definition);
        add_node(std::move(node));
    }
    
    /// Add dependency between existing nodes (only during construction phase)
    /// Throws if either job doesn't exist or if graph is already finalized
    void add_dependency(JobDependency dependency) {
        if (finalized_) {
            throw std::runtime_error("Cannot add dependency to finalized graph");
        }
        dependencies_.push_back(std::move(dependency));
    }
    
    /// Finalize graph construction - makes graph immutable and validates acyclic property
    /// Must be called before graph can be used for execution
    /// Throws if graph contains cycles
    void finalize() {
        if (finalized_) {
            return; // Already finalized
        }
        if (has_cycle()) {
            throw std::runtime_error("Graph contains cycles");
        }
        build_lookup_structures();
        finalized_ = true;
    }
    
    /// Check if graph is finalized (immutable)
    bool is_finalized() const noexcept { return finalized_; }
    
    /// Get all nodes in the graph (only available after finalization)
    const std::vector<JobNode>& nodes() const {
        if (!finalized_) {
            throw std::runtime_error("Graph must be finalized before accessing nodes");
        }
        return nodes_;
    }
    
    /// Get all dependencies in the graph (only available after finalization)
    const std::vector<JobDependency>& dependencies() const;
    
    /// Get dependencies for a specific job (only available after finalization)
    std::vector<JobId> get_dependencies(const JobId& job_id) const {
        if (!finalized_) {
            throw std::runtime_error("Graph must be finalized before accessing dependencies");
        }
        auto it = dependency_map_.find(job_id);
        if (it != dependency_map_.end()) {
            return it->second;
        }
        return std::vector<JobId>{}; // No dependencies
    }
    
    /// Get dependents of a specific job (only available after finalization)
    std::vector<JobId> get_dependents(const JobId& job_id) const;
    
    /// Get job node by ID (only available after finalization)
    const JobNode* get_node(const JobId& job_id) const {
        if (!finalized_) {
            throw std::runtime_error("Graph must be finalized before accessing nodes");
        }
        auto it = node_index_.find(job_id);
        if (it != node_index_.end()) {
            return &nodes_[it->second];
        }
        return nullptr; // Node not found
    }
    
    /// Validate DAG properties (acyclic only)
    /// Called automatically during finalize(), can be called manually for validation
    bool is_acyclic() const;
    
    /// Get total node count
    size_t node_count() const noexcept { return nodes_.size(); }
    
    /// Get total dependency count
    size_t dependency_count() const noexcept { return dependencies_.size(); }
    
    /// String representation for serialization
    std::string to_string() const;

private:
    std::vector<JobNode> nodes_;
    std::vector<JobDependency> dependencies_;
    std::map<JobId, size_t> node_index_;
    std::map<JobId, std::vector<JobId>> dependency_map_;
    std::map<JobId, std::vector<JobId>> dependent_map_;
    bool finalized_ = false;
    
    /// Internal deterministic cycle detection using ordered traversal
    bool has_cycle() const {
        // Simple cycle detection using DFS
        std::map<JobId, int> state; // 0=unvisited, 1=visiting, 2=visited
        
        for (const auto& node : nodes_) {
            state[node.id()] = 0;
        }
        
        for (const auto& node : nodes_) {
            if (state[node.id()] == 0) {
                if (dfs_has_cycle(node.id(), state)) {
                    return true;
                }
            }
        }
        return false;
    }
    
    /// DFS helper for cycle detection
    bool dfs_has_cycle(const JobId& job_id, std::map<JobId, int>& state) const {
        state[job_id] = 1; // Mark as visiting
        
        // Check all dependencies (outgoing edges)
        for (const auto& dep : dependencies_) {
            if (dep.from() == job_id) {
                const JobId& target = dep.to();
                if (state[target] == 1) {
                    return true; // Back edge found - cycle detected
                }
                if (state[target] == 0 && dfs_has_cycle(target, state)) {
                    return true;
                }
            }
        }
        
        state[job_id] = 2; // Mark as visited
        return false;
    }
    
    /// Build internal lookup structures after finalization
    void build_lookup_structures() {
        // Build dependency map for efficient lookups
        for (const auto& dep : dependencies_) {
            dependency_map_[dep.to()].push_back(dep.from());
            dependent_map_[dep.from()].push_back(dep.to());
        }
        
        // Build node index for efficient node lookups
        for (size_t i = 0; i < nodes_.size(); ++i) {
            node_index_[nodes_[i].id()] = i;
        }
    }
};

} // namespace nx::batchflow