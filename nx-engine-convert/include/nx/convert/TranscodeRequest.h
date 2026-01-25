#pragma once
#include <cstdint>

namespace nx::convert {

/**
 * Placeholder LogicalClock for Phase 1.A compile-only skeleton
 * Will be replaced with proper nx-core LogicalClock in later phases
 */
struct LogicalClock final {
    uint64_t ticks;
    
    constexpr bool operator==(const LogicalClock&) const = default;
};

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
    LogicalClock clock;
    uint64_t request_id;

    // opaque handles only - NO LOGIC — PHASE 1.A
    uint64_t input_container_id;
    uint64_t target_format_id;

    constexpr bool operator==(const TranscodeRequest&) const = default;
    constexpr bool operator!=(const TranscodeRequest&) const = default;
};

}