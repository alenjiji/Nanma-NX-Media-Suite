#include <nx/audio/AudioEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::audio;

int main() {
    std::cout << "NX-Audio Determinism Tests\n";
    AudioEngine engine;
    
    AudioRequest request{
        LogicalClock{1},
        42,
        100,
        200
    };
    
    // Proof 1: Same input → same output
    auto r1 = engine.prepare(request);
    auto r2 = engine.prepare(request);
    assert(r1 == r2);
    std::cout << "Same input = same output: PASS\n";
    
    // Proof 2: No hidden time dependency (LogicalClock only)
    AudioEngine engine2;
    auto r3 = engine2.prepare(request);
    assert(r1 == r3);
    std::cout << "No hidden time dependency: PASS\n";
    
    // Proof 3: Order stability
    AudioRequest req_other{
        LogicalClock{2},
        43,
        101,
        201
    };
    
    auto ra = engine.prepare(request);
    auto rb = engine.prepare(req_other);
    auto rc = engine.prepare(request);
    auto rd = engine.prepare(req_other);
    
    assert(ra == rc); // Same request = same result
    assert(rb == rd); // Order doesn't matter
    std::cout << "Order stability: PASS\n";
    
    std::cout << "All tests PASSED\n";
    return 0;
}