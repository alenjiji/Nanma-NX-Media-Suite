#include "nx/audio/AudioEngine.h"

namespace nx::audio {

// PHASE 1.A — DETERMINISTIC API DEFINITION
// Contract-only implementation — NO LOGIC
AudioResult AudioEngine::prepare(const AudioRequest& request) const {
    (void)request;

    // Deterministic stub failure (Phase 1)
    return AudioResult{false, AudioOutcome{0, 0}};
}

AudioGraph AudioEngine::build_graph(const AudioRequest& request) const {
    (void)request;
    return AudioGraph{};
}

} // namespace nx::audio