#include "nx/convert/TranscodeEngine.h"
#include "nx/convert/ConvertError.h"

namespace nx::convert {

TranscodeResultType TranscodeEngine::prepare(const TranscodeRequest& request) const {
    (void)request;
    // Return deterministic failure with engine-specific error code only
    return TranscodeResult{false, TranscodeOutcome{0, 0}};
}

}