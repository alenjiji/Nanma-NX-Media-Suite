#pragma once
#include <cstdint>

namespace nx::convert {

using GraphId = uint64_t;
using NodeId = uint64_t;

enum class GraphNodeType : uint8_t {
    InputContainer,
    StreamAnalyzer,
    PolicyResolver,
    GraphBuilder,
    Executor,
    OutputContainer
};

}