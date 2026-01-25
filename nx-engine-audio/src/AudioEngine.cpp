#include "nx/audio/AudioEngine.h"
#include "nx/audio/AudioError.h"

namespace nx::audio {

AudioResult AudioEngine::prepare(const AudioRequest& request) const {
    (void)request;
    // Return deterministic failure with engine-specific error code only
    return nx::core::fail<AudioOutcome>(
        nx::core::Error::create(
            static_cast<nx::core::ErrorCode>(AudioError::NotImplemented)
        )
    );
}

AudioGraph AudioEngine::build_graph(const AudioRequest&) const {
    return AudioGraph{};
}

}