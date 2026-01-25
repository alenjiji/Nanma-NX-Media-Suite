#pragma once
#include <cstdint>

namespace nx::video {

struct LogicalClock final {
    uint64_t ticks;
    constexpr bool operator==(const LogicalClock&) const = default;
};

struct VideoProcessRequest final {
    LogicalClock clock;
    uint64_t request_id;
    uint64_t input_stream_id;
    uint64_t target_pipeline_id;

    constexpr bool operator==(const VideoProcessRequest&) const = default;
    constexpr bool operator!=(const VideoProcessRequest&) const = default;
};

}