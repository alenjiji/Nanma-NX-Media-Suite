#include "nx/convert/TranscodeEngine.h"

namespace nx::convert {

// PHASE 1.A — DETERMINISTIC API DEFINITION
// Contract-only implementation - no logic
TranscodeResult TranscodeEngine::prepare(const TranscodeRequest& request) const {
    (void)request;
    // Return deterministic failure - Phase 1.A skeleton has no logic
    return TranscodeResult{false, TranscodeOutcome{0, 0}};
}

// PHASE 1.A — DETERMINISTIC API DEFINITION
TranscodeGraph TranscodeEngine::build_graph(const TranscodeRequest&) const {
    // Return deterministic empty graph structure
    return TranscodeGraph{};
}

}