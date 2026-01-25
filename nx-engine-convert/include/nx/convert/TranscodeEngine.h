#pragma once
#include "TranscodeRequest.h"
#include "TranscodeResult.h"

namespace nx::convert {

class TranscodeEngine {
public:
    TranscodeEngine() = default;
    
    TranscodeResultType prepare(const TranscodeRequest& request) const;
};

}