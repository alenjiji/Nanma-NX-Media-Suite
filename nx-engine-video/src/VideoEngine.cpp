#include "nx/video/VideoEngine.h"

namespace nx::video {

// PHASE 1.A — ENGINE SKELETON (NO LOGIC)
VideoProcessResult VideoEngine::construct_pipeline(const VideoProcessRequest& request) const {
    (void)request;
    return VideoProcessResult{false, VideoProcessOutcome{0, 0}};
}

}