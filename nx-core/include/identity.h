#pragma once

#include <array>
#include <string>
#include <string_view>
#include <cstdint>

namespace nx::core {

/**
 * Deterministic identity system for Nanma NX-MediaSuite
 * 
 * All IDs are content-derived using SHA-256 hashing to ensure:
 * - Same inputs produce identical IDs across runs, machines, and time
 * - Different inputs produce different IDs (collision-resistant)
 * - No dependency on timestamps, randomness, or hardware-specific behavior
 * - Thread-safe by construction (immutable after creation)
 */

// 256-bit hash represented as 32 bytes
using HashBytes = std::array<uint8_t, 32>;

/**
 * Base class for all deterministic identities
 * Provides common functionality for hash-based IDs
 */
class Identity {
public:
    // Comparison operators for deterministic ordering
    bool operator==(const Identity& other) const noexcept;
    bool operator!=(const Identity& other) const noexcept;
    bool operator<(const Identity& other) const noexcept;
    
    // Serialization support
    std::string to_string() const;
    const HashBytes& bytes() const noexcept { return hash_; }
    
    // Compute SHA-256 hash of input data (public for external use)
    static HashBytes compute_hash(std::string_view input);

protected:
    explicit Identity(const HashBytes& hash) : hash_(hash) {}
    
private:
    HashBytes hash_;
};

/**
 * JobID - Deterministic identifier for processing jobs
 * 
 * Content-derived from job parameters, input specifications, and processing configuration.
 * Same job definition always produces the same JobID.
 * 
 * Example:
 *   JobID id1 = JobID::from_content("transcode|input.mp4|h264|1920x1080");
 *   JobID id2 = JobID::from_content("transcode|input.mp4|h264|1920x1080");
 *   assert(id1 == id2); // Same content → same ID
 */
class JobID : public Identity {
public:
    // Create JobID from content string (job parameters, inputs, configuration)
    static JobID from_content(std::string_view content);
    
private:
    explicit JobID(const HashBytes& hash) : Identity(hash) {}
};

/**
 * RunID - Deterministic identifier for execution runs
 * 
 * Content-derived from run parameters, environment specification, and execution context.
 * Same run configuration always produces the same RunID.
 * 
 * Example:
 *   RunID id1 = RunID::from_content("batch_v1.0|preset_broadcast|2024-01-01");
 *   RunID id2 = RunID::from_content("batch_v1.0|preset_broadcast|2024-01-01");
 *   assert(id1 == id2); // Same content → same ID
 */
class RunID : public Identity {
public:
    // Create RunID from content string (run parameters, environment, context)
    static RunID from_content(std::string_view content);
    
private:
    explicit RunID(const HashBytes& hash) : Identity(hash) {}
};

/**
 * NodeID - Deterministic identifier for graph nodes
 * 
 * Content-derived from node type, parameters, and configuration.
 * Same node specification always produces the same NodeID.
 * 
 * Example:
 *   NodeID id1 = NodeID::from_content("decode|h264|baseline_profile");
 *   NodeID id2 = NodeID::from_content("decode|h264|baseline_profile");
 *   assert(id1 == id2); // Same content → same ID
 */
class NodeID : public Identity {
public:
    // Create NodeID from content string (node type, parameters, configuration)
    static NodeID from_content(std::string_view content);
    
private:
    explicit NodeID(const HashBytes& hash) : Identity(hash) {}
};

/**
 * ArtifactID - Deterministic identifier for processing artifacts
 * 
 * Content-derived from artifact content, metadata, and generation parameters.
 * Same artifact specification always produces the same ArtifactID.
 * 
 * Example:
 *   ArtifactID id1 = ArtifactID::from_content("report|conversion|job123|success");
 *   ArtifactID id2 = ArtifactID::from_content("report|conversion|job123|success");
 *   assert(id1 == id2); // Same content → same ID
 */
class ArtifactID : public Identity {
public:
    // Create ArtifactID from content string (artifact content, metadata, parameters)
    static ArtifactID from_content(std::string_view content);
    
private:
    explicit ArtifactID(const HashBytes& hash) : Identity(hash) {}
};

} // namespace nx::core