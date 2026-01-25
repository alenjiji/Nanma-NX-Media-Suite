#pragma once

namespace nx::audio {

enum class AudioError : uint32_t {
    InvalidRequest = 1,
    GraphConstructionFailed = 2,
    DeterminismViolation = 3,
    ForbiddenOperation = 4
};

}