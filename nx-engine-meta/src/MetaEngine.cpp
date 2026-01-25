#include "nx/meta/MetaEngine.h"

namespace nx::meta {

// NO LOGIC — PHASE 1.A
// Compile-only skeleton - no metadata repair logic
// Media essence is read-only by default
// Deterministic by construction
MetaRepairResult MetaEngine::plan_repair(const MetaRepairRequest& request) const {
    (void)request;
    // Return deterministic failure - no repair planning logic in Phase 1.A
    return nx::core::fail<MetaRepairOutcome>(
        nx::core::Error::create(
            nx::core::ErrorCode::NotImplemented,
            nx::core::ErrorSeverity::Error,
            "Phase 1.A skeleton - no metadata repair planning logic"
        )
    );
}

}