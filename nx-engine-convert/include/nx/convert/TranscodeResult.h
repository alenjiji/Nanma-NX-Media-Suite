#pragma once
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
 * Placeholder Result type for Phase 1.A compile-only skeleton
 * Will be replaced with proper nx-core Result system in later phases
 */
struct TranscodeResult final {
    bool success;
    TranscodeOutcome outcome;
    
    constexpr bool operator==(const TranscodeResult&) const = default;
    constexpr bool is_success() const { return success; }
    constexpr bool is_failure() const { return !success; }
};

}