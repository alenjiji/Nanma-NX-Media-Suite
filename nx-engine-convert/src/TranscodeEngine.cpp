#include "nx/convert/TranscodeEngine.h"

namespace nx::convert {

// PHASE 1.A — DETERMINISTIC API DEFINITION
// Contract-only implementation - no logic
TranscodeResult TranscodeEngine::prepare(const TranscodeRequest& request) const {
    (void)request;

    // Deterministic failure stub
    return TranscodeResult{
        false,
        TranscodeOutcome{0, 0}
    };
}

TranscodeGraph TranscodeEngine::build_graph(const TranscodeRequest&) const {
    return TranscodeGraph{};
}

}
