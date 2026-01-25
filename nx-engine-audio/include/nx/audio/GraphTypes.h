#pragma once
#include <cstdint>

namespace nx::audio {

using GraphId = uint64_t;
using NodeId = uint64_t;

enum class AudioNodeType : uint8_t {
    DecodeNode,
    SRCNode,
    GainNode,
    LoudnessNode,
    DitherNode,
    EncodeNode
};

}