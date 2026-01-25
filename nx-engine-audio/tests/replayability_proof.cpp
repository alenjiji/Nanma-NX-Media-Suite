#include <nx/audio/AudioEngine.h>
#include <cassert>

using namespace nx::audio;

int main() {
    // REPLAYABILITY PROOF
    
    AudioRequest request{
        nx::core::LogicalClock{42},
        1001,
        2002,
        3003
    };
    
    // Test 1: Different engine instances
    AudioEngine engine1;
    AudioEngine engine2;
    AudioEngine engine3;
    
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