#pragma once
#include <cstdint>

namespace nx::convert {

enum class ConvertError : uint32_t {
    NotImplemented = 1000,
    InvalidInput = 1001,
    InvalidFormat = 1002
};

}