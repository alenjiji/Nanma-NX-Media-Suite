#pragma once
#include <nx-core/include/result.h>
#include <cstdint>

namespace nx::convert {

/**
 * Deterministic transcode outcome - pure value type
 * 
 * Contract:
 * - No side effects
 * - Fully comparable for determinism
 * - Immutable after construction
 */
struct TranscodeOutcome final {
    uint64_t graph_id;
    uint64_t verification_token;

    constexpr bool operator==(const TranscodeOutcome&) const = default;
    constexpr bool operator!=(const TranscodeOutcome&) const = default;
};

/**
 * Result type for transcode operations
 * Uses nx-core Result system for deterministic error handling
 */
using TranscodeResult = nx::core::Result<TranscodeOutcome>;

}