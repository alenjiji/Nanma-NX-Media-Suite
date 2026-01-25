#pragma once
#include "VideoProcessRequest.h"
#include "VideoProcessResult.h"
#include "Errors.h"

namespace nx::video {

/**
 * NX-VideoTrans Core Engine
 * 
 * PHASE 1.A — ENGINE SKELETON (NO LOGIC)
 * 
 * Pure coordinator for deterministic video pipeline construction.
 * Enforces deterministic execution and explicit pipeline definition.
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
 * - Video encoding/decoding logic
 * - Color processing logic
 * - Frame processing
 * - Pipeline execution
 * - System time dependencies
 */
class VideoEngine final {
public:
    explicit VideoEngine() = default;

    /**
     * Construct video pipeline - deterministic, no side effects
     * 
     * Contract:
     * - Deterministic: same request = same result
     * - No side effects: does not modify any state
     * - All state via parameters: no hidden dependencies
     * 
     * @param request Video pipeline construction parameters (value type)
     * @return Result containing pipeline outcome or explicit error
     */
    VideoProcessResult construct_pipeline(const VideoProcessRequest& request) const;
};

}