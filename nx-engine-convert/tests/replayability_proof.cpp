#include <nx/convert/TranscodeEngine.h>
#include <cassert>

using namespace nx::convert;

int main() {
    // REPLAYABILITY PROOF
    
    TranscodeRequest request{
        LogicalClock{42},
        InputContainerId{1001},
        OutputContainerId{2002},
        TargetFormatId{3003}
    };
    
    // Test 1: Different engine instances
    TranscodeEngine engine1;
    TranscodeEngine engine2;
    TranscodeEngine engine3;
    
    auto result1 = engine1.prepare(request);
    auto result2 = engine2.prepare(request);
    auto result3 = engine3.prepare(request);
    
    assert(result1 == result2);
    assert(result2 == result3);
    
    // Test 2: Re-run identical requests (simulating different times)
    auto run1 = engine1.prepare(request);
    auto run2 = engine1.prepare(request);
    auto run3 = engine1.prepare(request);
    
    assert(run1 == run2);
    assert(run2 == run3);
    
    // Test 3: Cross-instance replayability
    auto cross1 = engine1.prepare(request);
    auto cross2 = engine2.prepare(request);
    auto cross3 = engine3.prepare(request);
    
    assert(cross1 == cross2);
    assert(cross2 == cross3);
    assert(cross1 == cross3);
    
    // Test 4: Multiple identical requests in sequence
    for (int i = 0; i < 10; ++i) {
        auto replay = engine1.prepare(request);
        assert(replay == result1);
    }
    
    // PROOF: Same input always produces identical output
    // No time dependencies, no randomness, no process state
    
    return 0;
}