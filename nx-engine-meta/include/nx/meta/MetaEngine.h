// 🔒 PHASE 1 API FREEZE
// This public API is frozen as of Phase 1 completion.
// Changes require explicit architectural review.
#pragma once
#include "MetaRepairRequest.h"
#include "MetaRepairResult.h"
#include "Errors.h"

namespace nx::meta {

/**
 * NX-MetaFix Core Engine
 * 
 * NO LOGIC — PHASE 1.A
 * Compile-only skeleton
 * Media essence is read-only by default
 * Deterministic by construction
 * 
 * Pure coordinator for metadata and container repair operations.
 * Enforces deterministic execution and explicit repair planning.
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
 * - Processing or repair logic implementation
 * - Media essence modification in any form
 * - Codecs, remuxing, or format conversion
 * - IO, filesystem, or network operations
 * - Inference, heuristics, or automatic behavior
 * - Execution or scheduling logic
 * - System time dependencies
 */
class MetaEngine final {
public:
    explicit MetaEngine() = default;

    /**
     * Plan metadata repair operation - deterministic, no side effects
     * 
     * Contract:
     * - Deterministic: same request = same result
     * - No side effects: does not modify any state
     * - All state via parameters: no hidden dependencies
     * - Media essence is read-only by default
     * 
     * @param request Metadata repair planning parameters (value type)
     * @return Result containing repair outcome or explicit error
     */
    MetaRepairResult plan_repair(const MetaRepairRequest& request) const;
};

}