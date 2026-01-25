#pragma once
#include <cstdint>

namespace nx::audio {

enum class AudioError : uint32_t {
    NotImplemented = 2000,
    InvalidInput = 2001,
    InvalidFormat = 2002,
    InvalidAudioId = 2003
};

}