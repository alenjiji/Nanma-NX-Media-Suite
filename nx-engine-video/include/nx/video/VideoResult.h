#pragma once
#include <cstdint>
#include <result.h>

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
 * Video processing result using nx::core::Result<T>
 * Complies with architectural principles for error handling
 */
using VideoResult = nx::core::Result<VideoOutcome>;

}