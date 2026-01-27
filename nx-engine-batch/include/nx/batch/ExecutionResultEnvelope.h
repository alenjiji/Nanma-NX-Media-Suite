#pragma once

#include "SessionTypes.h"
#include <vector>
#include <string>

namespace nx::batch {

/**
 * Declared output descriptor for execution results
 * 
 * Contains only path and type information - no actual data or verification.
 */
struct OutputDescriptor {
    std::string path;  // Declared output path
    std::string type;  // Output type (e.g., "video", "audio", "metadata")
    
    bool operator==(const OutputDescriptor& other) const = default;
};

/**
 * Verification artifact placeholder
 * 
 * Placeholder for future verification data - contains no actual verification.
 */
struct VerificationArtifact {
    std::string artifact_id;  // Opaque artifact identifier
    std::string artifact_type; // Type of verification (e.g., "checksum", "format")
    
    bool operator==(const VerificationArtifact& other) const = default;
};

/**
 * Immutable execution result envelope
 * 
 * Contains only structural information about execution completion.
 * 
 * EXPLICITLY FORBIDDEN:
 * - Progress indicators
 * - Error codes or success/failure booleans
 * - Monitor annotations or metadata
 * - Timing information
 * - Resource usage data
 * - Log entries or diagnostic information
 * 
 * ALLOWED CONTENTS:
 * - Session and job identity
 * - Declared output descriptors (paths/types only)
 * - Verification artifact placeholders (no actual data)
 */
struct ExecutionResultEnvelope {
    SessionId session_id;                               // REFERENCED: Session identity
    SessionJobId job_id;                               // REFERENCED: Job identity
    std::vector<OutputDescriptor> declared_outputs;    // COPIED: Output path/type declarations
    std::vector<VerificationArtifact> verification_placeholders; // COPIED: Verification placeholders
    
    bool operator==(const ExecutionResultEnvelope& other) const = default;
};

} // namespace nx::batch