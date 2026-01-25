#include <nx/convert/TranscodeEngine.h>
#include <cassert>

using namespace nx::convert;

int main() {
    // Test data - identical requests
    TranscodeRequest req1{
        LogicalClock{42},
        InputContainerId{1001},
        OutputContainerId{2001},
        TargetFormatId{3001}
    };
    
    TranscodeRequest req2{
        LogicalClock{42},
        InputContainerId{1001},
        OutputContainerId{2001},
        TargetFormatId{3001}
    };
    
    assert(req1 == req2); // Verify test data identical
    
    // Same-input / same-output test
    TranscodeEngine engine1;
    auto result1a = engine1.prepare(req1);
    auto result1b = engine1.prepare(req1);
    assert(result1a == result1b);
    
    // Cross-instance determinism test
    TranscodeEngine engine2;
    auto result2 = engine2.prepare(req1);
    assert(result1a == result2);
    
    // Repeatability test (N=5)
    auto r1 = engine1.prepare(req1);
    auto r2 = engine1.prepare(req1);
    auto r3 = engine1.prepare(req1);
    auto r4 = engine1.prepare(req1);
    auto r5 = engine1.prepare(req1);
    
    assert(r1 == r2);
    assert(r2 == r3);
    assert(r3 == r4);
    assert(r4 == r5);
    
    // Different call order test
    TranscodeRequest other{
        LogicalClock{99},
        InputContainerId{9999},
        OutputContainerId{8888},
        TargetFormatId{7777}
    };
    
    auto a1 = engine1.prepare(req1);
    auto b1 = engine1.prepare(other);
    auto a2 = engine1.prepare(req1);
    auto b2 = engine1.prepare(other);
    
    assert(a1 == a2); // Same request = same result
    assert(b1 == b2); // Order doesn't matter
    
    return 0;
}