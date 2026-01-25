#pragma once
#include <nx-core/include/logical_clock.h>
#include <cstdint>

namespace nx::video {

/**
 * Deterministic video processing request - pure value type
 * 
 * Contract:
 * - No filesystem paths or identifiers
 * - No mutable state
 * - Fully comparable for determinism
 * - All state passed via parameters
 */
struct VideoRequest final {
    nx::core::LogicalClock clock;
    uint64_t request_id;

    // opaque handles only - NO LOGIC — PHASE 1.A
    uint64_t input_video_id;
    uint64_t target_format_id;

    constexpr bool operator==(const VideoRequest&) const = default;
    constexpr bool operator!=(const VideoRequest&) const = default;
};

}