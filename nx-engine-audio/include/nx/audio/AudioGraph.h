#pragma once
#include "GraphTypes.h"
#include <vector>

namespace nx::audio {

struct AudioNode final {
    NodeId id;
    AudioNodeType type;

    constexpr bool operator==(const AudioNode&) const = default;
};

struct AudioGraph final {
    GraphId id;
    std::vector<AudioNode> nodes;

    constexpr bool operator==(const AudioGraph&) const = default;
};

}