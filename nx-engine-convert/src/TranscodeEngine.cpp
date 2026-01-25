#include "nx/convert/TranscodeEngine.h"

namespace nx::convert {

// PHASE 1.A — DETERMINISTIC API DEFINITION
// Contract-only implementation - no logic
TranscodeResult TranscodeEngine::prepare(const TranscodeRequest& request) const {
    (void)request;
    // Return deterministic failure using engine-specific error
    // Same input always produces same error result
    return nx::core::fail<TranscodeOutcome>(
        nx::core::Error::create(
            nx::core::ErrorCode::NotImplemented,
            nx::core::ErrorSeverity::Error,
            "Phase 1.A deterministic API - no execution logic"
        )
    );
}

// PHASE 1.A — DETERMINISTIC API DEFINITION
TranscodeGraph TranscodeEngine::build_graph(const TranscodeRequest&) const {
    // Return deterministic empty graph structure
    return TranscodeGraph{};
}

}