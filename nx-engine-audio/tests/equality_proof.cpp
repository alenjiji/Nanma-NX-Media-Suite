#include <nx/audio/AudioEngine.h>
#include <cassert>

using namespace nx::audio;

int main() {
    // Test data - identical requests with strong ID types
    AudioRequest req1{
        nx::core::LogicalClock{42},
        1001,
        2001,
        3001
    };
    
    AudioRequest req2{
        nx::core::LogicalClock{42},
        1001,
        2001,
        3001
    };
    
    assert(req1 == req2); // Verify test data identical
    
    // Same-input / same-output test
    AudioEngine engine1;
    auto result1a = engine1.prepare(req1);
    auto result1b = engine1.prepare(req1);
    assert(result1a == result1b);
    
    // Cross-instance determinism test
    AudioEngine engine2;
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
    AudioRequest other{
        nx::core::LogicalClock{99},
        9999,
        8888,
        7777
    };
    
    auto a1 = engine1.prepare(req1);
    auto b1 = engine1.prepare(other);
    auto a2 = engine1.prepare(req1);
    auto b2 = engine1.prepare(other);
    
    assert(a1 == a2); // Same request = same result
    assert(b1 == b2); // Order doesn't matter
    
    return 0;
}