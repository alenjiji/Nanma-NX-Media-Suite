// 🔒 PHASE 1 API FREEZE
// This public API is frozen as of Phase 1 completion.
// Changes require explicit architectural review.
#pragma once
#include "VideoRequest.h"
#include "VideoResult.h"
#include "VideoGraph.h"
#include "Errors.h"

namespace nx::video {

/**
 * NX-VideoTrans Core Engine
 * 
 * PHASE 1.A — ENGINE SKELETON (NO LOGIC)
 * 
 * Pure coordinator for color-safe, deterministic video processing operations.
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
 * - Video format I/O operations
 * - Codec implementations
 * - Color space processing
 * - Scaling operations
 * - Encoder implementations
 * - System time dependencies
 */
class VideoEngine final {
public:
    explicit VideoEngine() = default;

    /**
     * Prepare video processing operation - deterministic, no side effects
     * 
     * Contract:
     * - Deterministic: same request = same result
     * - No side effects: does not modify any state
     * - All state via parameters: no hidden dependencies
     * 
     * @param request Complete video processing parameters (value type)
     * @return Result containing outcome or explicit error
     */
    VideoResult prepare(const VideoRequest& request) const;

private:
    /**
     * Build video processing graph - deterministic, no side effects
     * 
     * Contract:
     * - Pure function: same input = same output
     * - No state modification
     * 
     * @param request Video processing parameters
     * @return Graph data structure (no behavior)
     */
    VideoGraph build_graph(const VideoRequest& request) const;
};

}