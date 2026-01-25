#pragma once
#include <nx-core/include/result.h>
#include <cstdint>

namespace nx::video {

/**
 * Deterministic video processing outcome - pure value type
 * 
 * Contract:
 * - No side effects
 * - Fully comparable for determinism
 * - Immutable after construction
 */
struct VideoOutcome final {
    uint64_t graph_id;
    uint64_t verification_token;

    constexpr bool operator==(const VideoOutcome&) const = default;
    constexpr bool operator!=(const VideoOutcome&) const = default;
};

/**
 * Result type for video processing operations
 * Uses nx-core Result system for deterministic error handling
 */
using VideoResult = nx::core::Result<VideoOutcome>;

}