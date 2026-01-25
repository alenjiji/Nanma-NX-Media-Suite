#pragma once
#include <nx-core/include/logical_clock.h>
#include <cstdint>

namespace nx::convert {

/**
 * Deterministic transcode request - pure value type
 * 
 * Contract:
 * - No filesystem paths or identifiers
 * - No mutable state
 * - Fully comparable for determinism
 * - All state passed via parameters
 */
struct TranscodeRequest final {
    nx::core::LogicalClock clock;
    uint64_t request_id;

    // opaque handles only - NO LOGIC — PHASE 1.A
    uint64_t input_container_id;
    uint64_t target_format_id;

    constexpr bool operator==(const TranscodeRequest&) const = default;
    constexpr bool operator!=(const TranscodeRequest&) const = default;
};

}