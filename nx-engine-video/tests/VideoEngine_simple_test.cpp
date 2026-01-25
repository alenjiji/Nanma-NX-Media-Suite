#include <nx/video/VideoEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::video;

// PHASE 1.B — DETERMINISM PROOF (NO SEMANTICS)
int main() {
    std::cout << "NX-Video Simple Determinism Test\n";
    VideoEngine engine;
    
    VideoRequest request{
        nx::core::LogicalClock{1},
        42,
        100,
        200
    };
    
    auto r1 = engine.prepare(request);
    auto r2 = engine.prepare(request);
    assert(r1 == r2);
    std::cout << "Same input = same output: PASS\n";
    
    std::cout << "Test PASSED\n";
    return 0;
}