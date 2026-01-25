#pragma once
#include <cstdint>

namespace nx::video {

using GraphId = uint64_t;
using NodeId = uint64_t;

enum class VideoNodeType : uint8_t {
    Demux,
    Decode,
    ColorSpaceNode,
    ScaleNode,
    FrameQueue,
    EncoderNode,
    Mux
};

}