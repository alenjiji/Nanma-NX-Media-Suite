#pragma once

namespace nx::video {

enum class VideoError : uint32_t {
    InvalidRequest = 1,
    GraphConstructionFailed = 2,
    DeterminismViolation = 3,
    ForbiddenOperation = 4
};

}