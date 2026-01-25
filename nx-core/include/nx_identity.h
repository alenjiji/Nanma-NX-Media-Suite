#pragma once

#include <array>
#include <string>
#include <string_view>
#include <cstdint>

namespace nx::core {

// 256-bit hash represented as 32 bytes
using HashBytes = std::array<uint8_t, 32>;

/**
 * JobID - Deterministic identifier for processing jobs
 * Content-derived from job parameters, input specifications, and processing configuration.
 * Same job definition always produces the same JobID.
 */
struct JobID final {
    HashBytes hash;
    
    static JobID from_content(std::string_view content);
    
    bool operator==(const JobID&) const noexcept = default;
    bool operator!=(const JobID&) const noexcept = default;
    bool operator<(const JobID& other) const noexcept;
    std::string to_string() const;
};

/**
 * RunID - Deterministic identifier for execution runs
 * Content-derived from run parameters, environment specification, and execution context.
 * Same run configuration always produces the same RunID.
 */
struct RunID final {
    HashBytes hash;
    
    static RunID from_content(std::string_view content);
    
    bool operator==(const RunID&) const noexcept = default;
    bool operator!=(const RunID&) const noexcept = default;
    bool operator<(const RunID& other) const noexcept;
    std::string to_string() const;
};

/**
 * NodeID - Deterministic identifier for graph nodes
 * Content-derived from node type, parameters, and configuration.
 * Same node specification always produces the same NodeID.
 */
struct NodeID final {
    HashBytes hash;
    
    static NodeID from_content(std::string_view content);
    
    bool operator==(const NodeID&) const noexcept = default;
    bool operator!=(const NodeID&) const noexcept = default;
    bool operator<(const NodeID& other) const noexcept;
    std::string to_string() const;
};

/**
 * ArtifactID - Deterministic identifier for processing artifacts
 * Content-derived from artifact content, metadata, and generation parameters.
 * Same artifact specification always produces the same ArtifactID.
 */
struct ArtifactID final {
    HashBytes hash;
    
    static ArtifactID from_content(std::string_view content);
    
    bool operator==(const ArtifactID&) const noexcept = default;
    bool operator!=(const ArtifactID&) const noexcept = default;
    bool operator<(const ArtifactID& other) const noexcept;
    std::string to_string() const;
};

} // namespace nx::core