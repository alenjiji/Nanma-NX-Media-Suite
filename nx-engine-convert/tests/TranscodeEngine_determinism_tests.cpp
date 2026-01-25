#include <nx/convert/TranscodeEngine.h>
#include <cassert>

using namespace nx::convert;

int main() {
    TranscodeEngine engine;
    
    TranscodeRequest request{
        LogicalClock{42},
        InputContainerId{1001},
        OutputContainerId{2002},
        TargetFormatId{3003}
    };
    
    // Proof 1: Same input → same output
    auto r1 = engine.prepare(request);
    auto r2 = engine.prepare(request);
    assert(r1 == r2);
    
    // Proof 2: No hidden time dependency (LogicalClock only)
    TranscodeEngine engine2;
    auto r3 = engine2.prepare(request);
    assert(r1 == r3);
    
    // Proof 3: Order stability
    TranscodeRequest req_other{
        LogicalClock{99},
        InputContainerId{9001},
        OutputContainerId{9002},
        TargetFormatId{9003}
    };
    
    auto ra = engine.prepare(request);
    auto rb = engine.prepare(req_other);
    auto rc = engine.prepare(request);
    auto rd = engine.prepare(req_other);
    
    assert(ra == rc); // Same request = same result
    assert(rb == rd); // Order doesn't matter
    
    return 0;
}