#pragma once
#include <nx-core/include/logical_clock.h>
#include <cstdint>

namespace nx::audio {

/**
 * Deterministic audio processing request - pure value type
 * 
 * Contract:
 * - No filesystem paths or identifiers
 * - No mutable state
 * - Fully comparable for determinism
 * - All state passed via parameters
 */
struct AudioRequest final {
    nx::core::LogicalClock clock;
    uint64_t request_id;

    // opaque handles only - NO LOGIC — PHASE 1.A
    uint64_t input_audio_id;
    uint64_t target_format_id;

    constexpr bool operator==(const AudioRequest&) const = default;
    constexpr bool operator!=(const AudioRequest&) const = default;
};

}