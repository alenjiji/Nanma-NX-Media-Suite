// 🔒 ARCHITECTURAL DETERMINISM TEST
// Phase 1 invariant — MUST NOT be weakened.
// Modifications require architectural review.
#include <nx/video/VideoEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::video;

// PHASE 1.B — MINIMAL DETERMINISM PROOF (NO SEMANTICS)
int main() {
    std::cout << "NX-Video Minimal Determinism Proof\n";
    VideoEngine engine;
    
    VideoRequest req{
        nx::core::LogicalClock{1},
        42,
        100,
        200
    };
    
    // Canonical determinism proof
    auto r1 = engine.prepare(req);
    auto r2 = engine.prepare(req);
    assert(r1 == r2);
    
    VideoEngine e2;
    auto r3 = e2.prepare(req);
    assert(r1 == r3);
    
    std::cout << "Determinism proof: PASS\n";
    return 0;
}