#pragma once
#include "GraphTypes.h"
#include <vector>

namespace nx::convert {

struct GraphNode final {
    NodeId id;
    GraphNodeType type;

    constexpr bool operator==(const GraphNode&) const = default;
};

struct TranscodeGraph final {
    GraphId id;
    std::vector<GraphNode> nodes;

    constexpr bool operator==(const TranscodeGraph&) const = default;
};

}