#pragma once
#include <cstdint>

namespace nx::convert {

struct LogicalClock final {
    uint64_t ticks;
    constexpr bool operator==(const LogicalClock&) const = default;
};

// Strong ID types - no semantic payload
struct InputContainerId {
    uint64_t value;
    constexpr bool operator==(const InputContainerId&) const = default;
};

struct OutputContainerId {
    uint64_t value;
    constexpr bool operator==(const OutputContainerId&) const = default;
};

struct TargetFormatId {
    uint64_t value;
    constexpr bool operator==(const TargetFormatId&) const = default;
};

struct TranscodeRequest final {
    LogicalClock clock;
    InputContainerId input_container;
    OutputContainerId output_container;
    TargetFormatId target_format;

    constexpr bool operator==(const TranscodeRequest&) const = default;
    constexpr bool operator!=(const TranscodeRequest&) const = default;
};

}