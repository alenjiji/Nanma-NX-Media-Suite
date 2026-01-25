// 🔒 PHASE 1 API FREEZE
// This public API is frozen as of Phase 1 completion.
// Changes require explicit architectural review.
#pragma once
#include "AudioRequest.h"
#include "AudioResult.h"
#include "AudioGraph.h"
#include "Errors.h"

namespace nx::audio {

/**
 * NX-AudioLab Core Engine
 * 
 * PHASE 1.A — DETERMINISTIC API DEFINITION
 * 
 * Pure coordinator for sample-accurate audio processing operations.
 * Enforces deterministic execution and explicit processing graphs.
 * 
 * Deterministic API Contract:
 * - All methods are deterministic (same input = same output)
 * - No side effects or hidden state
 * - All state passed via parameters
 * - Returns Result types only (no exceptions)
 * - Value types only (no mutable state)
 * - Uses LogicalClock only (no wall-clock time)
 * 
 * Forbidden:
 * - std::string paths or filesystem identifiers
 * - Static or global state
 * - Side effects
 * - Audio I/O operations
 * - DSP algorithm implementations
 * - System time dependencies
 */
class AudioEngine final {
public:
    explicit AudioEngine() = default;

    /**
     * Prepare audio processing operation - deterministic, no side effects
     * 
     * Contract:
     * - Deterministic: same request = same result
     * - No side effects: does not modify any state
     * - All state via parameters: no hidden dependencies
     * 
     * @param request Complete audio processing parameters (value type)
     * @return Result containing outcome or explicit error
     */
    AudioResult prepare(const AudioRequest& request) const;

private:
    /**
     * Build audio processing graph - deterministic, no side effects
     * 
     * Contract:
     * - Pure function: same input = same output
     * - No state modification
     * 
     * @param request Audio processing parameters
     * @return Graph data structure (no behavior)
     */
    AudioGraph build_graph(const AudioRequest& request) const;
};

}