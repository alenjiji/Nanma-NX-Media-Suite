#pragma once
#include "TranscodeRequest.h"
#include "TranscodeResult.h"
#include "TranscodeGraph.h"
#include "Errors.h"

namespace nx::convert {

/**
 * NX-Convert Pro Core Engine
 * 
 * PHASE 1.A — DETERMINISTIC API DEFINITION
 * 
 * Pure coordinator for lossless transcoding operations.
 * Enforces deterministic execution and explicit pipeline construction.
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
 * - IO operations
 * - System time dependencies
 */
class TranscodeEngine final {
public:
    explicit TranscodeEngine() = default;

    /**
     * Prepare transcode operation - deterministic, no side effects
     * 
     * Contract:
     * - Deterministic: same request = same result
     * - No side effects: does not modify any state
     * - All state via parameters: no hidden dependencies
     * 
     * @param request Complete transcode parameters (value type)
     * @return Result containing outcome or explicit error
     */
    TranscodeResult prepare(const TranscodeRequest& request) const;

private:
    /**
     * Build processing graph - deterministic, no side effects
     * 
     * Contract:
     * - Pure function: same input = same output
     * - No state modification
     * 
     * @param request Transcode parameters
     * @return Graph data structure (no behavior)
     */
    TranscodeGraph build_graph(const TranscodeRequest& request) const;
};

}