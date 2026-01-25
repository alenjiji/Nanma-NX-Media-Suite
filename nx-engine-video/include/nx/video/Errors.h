#pragma once

namespace nx::video {

/**
 * Video engine error enumeration
 * 
 * Contract:
 * - Stable error codes across versions
 * - No string dependencies
 * - Deterministic error classification
 * - Video-specific error semantics
 */
enum class VideoError : uint32_t {
    InvalidRequest = 1,
    PipelineConstructionFailed = 2,
    DeterminismViolation = 3,
    ForbiddenOperation = 4
};

}