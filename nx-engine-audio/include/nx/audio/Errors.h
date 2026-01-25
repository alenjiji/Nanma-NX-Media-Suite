#pragma once

namespace nx::audio {

/**
 * Explicit audio engine error enumeration
 * 
 * Contract:
 * - Stable error codes across versions
 * - No string dependencies
 * - Deterministic error classification
 * - Audio-specific error semantics
 */
enum class AudioError : uint32_t {
    InvalidRequest = 1,
    GraphConstructionFailed = 2,
    DeterminismViolation = 3,
    ForbiddenOperation = 4
};

}