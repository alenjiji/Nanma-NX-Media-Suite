#pragma once
#include <result.h>
#include <cstdint>

namespace nx::audio {

/**
 * Deterministic audio processing outcome - pure value type
 * 
 * Contract:
 * - No side effects
 * - Fully comparable for determinism
 * - Immutable after construction
 */
struct AudioOutcome final {
    uint64_t graph_id;
    uint64_t verification_token;

    constexpr bool operator==(const AudioOutcome&) const = default;
    constexpr bool operator!=(const AudioOutcome&) const = default;
};

/**
 * Result type for audio processing operations
 * Uses nx-core Result system for deterministic error handling
 */
using AudioResult = nx::core::Result<AudioOutcome>;

}