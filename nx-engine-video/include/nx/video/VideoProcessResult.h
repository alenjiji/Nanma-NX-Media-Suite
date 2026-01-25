#pragma once
#include <cstdint>

namespace nx::video {

struct VideoProcessOutcome final {
    uint64_t pipeline_id;
    uint64_t verification_token;

    constexpr bool operator==(const VideoProcessOutcome&) const = default;
    constexpr bool operator!=(const VideoProcessOutcome&) const = default;
};

struct VideoProcessResult final {
    bool success;
    VideoProcessOutcome outcome;
    
    constexpr bool operator==(const VideoProcessResult&) const = default;
    constexpr bool is_success() const { return success; }
    constexpr bool is_failure() const { return !success; }
};

}