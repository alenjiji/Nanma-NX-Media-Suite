#pragma once
#include "GraphTypes.h"
#include <vector>

namespace nx::video {

struct VideoNode final {
    NodeId id;
    VideoNodeType type;

    bool operator==(const VideoNode&) const = default;
};

struct VideoGraph final {
    GraphId id;
    std::vector<VideoNode> nodes;

    bool operator==(const VideoGraph&) const = default;
};

}