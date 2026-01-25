#pragma once

namespace nx::convert {

/**
 * Explicit engine error enumeration
 * 
 * Contract:
 * - Stable error codes across versions
 * - No string dependencies
 * - Deterministic error classification
 */
enum class TranscodeError : uint32_t {
    InvalidRequest = 1,
    GraphConstructionFailed = 2,
    DeterminismViolation = 3,
    ForbiddenOperation = 4
};

}