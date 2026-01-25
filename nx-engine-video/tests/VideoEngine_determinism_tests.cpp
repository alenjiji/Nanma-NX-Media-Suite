#include <nx/video/VideoEngine.h>
#include <cassert>

using namespace nx::video;

int main() {
    VideoEngine engine;
    
    VideoRequest request{
        nx::core::LogicalClock{1},
        42,
        100,
        200
    };
    
    // Proof 1: Same input → same output
    auto r1 = engine.prepare(request);
    auto r2 = engine.prepare(request);
    assert(r1 == r2);
    
    // Proof 2: No hidden time dependency (LogicalClock only)
    VideoEngine engine2;
    auto r3 = engine2.prepare(request);
    assert(r1 == r3);
    
    // Proof 3: Order stability
    VideoRequest req_other{
        nx::core::LogicalClock{2},
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
    
    return 0;
}