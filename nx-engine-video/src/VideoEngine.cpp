#include "nx/video/VideoEngine.h"

namespace nx::video {

// NO LOGIC — PHASE 1.A
// Contract-only implementation - no video processing logic
VideoResult VideoEngine::prepare(const VideoRequest& request) const {
    (void)request;
    // Return deterministic failure - no execution logic in Phase 1.A
    return nx::core::fail<VideoOutcome>(
        nx::core::Error::create(
            nx::core::ErrorCode::NotImplemented,
            nx::core::ErrorSeverity::Error,
            "Phase 1.A skeleton - no video processing logic"
        )
    );
}

// NO LOGIC — PHASE 1.A
VideoGraph VideoEngine::build_graph(const VideoRequest&) const {
    // Return deterministic empty graph structure
    return VideoGraph{};
}

}