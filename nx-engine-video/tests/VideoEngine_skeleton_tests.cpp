#include <nx/video/VideoEngine.h>
#include <cassert>

using namespace nx::video;

int main() {
    VideoEngine engine;
    
    VideoProcessRequest request{
        nx::core::LogicalClock{1},
        42,
        100,
        200
    };
    
    // Proof 1: Same input → same output
    auto r1 = engine.construct_pipeline(request);
    auto r2 = engine.construct_pipeline(request);
    assert(r1 == r2);
    
    // Proof 2: No hidden time dependency (LogicalClock only)
    VideoEngine engine2;
    auto r3 = engine2.construct_pipeline(request);
    assert(r1 == r3);
    
    // Proof 3: Order stability
    VideoProcessRequest req_other{
        nx::core::LogicalClock{2},
        43,
        101,
        201
    };
    
    auto ra = engine.construct_pipeline(request);
    auto rb = engine.construct_pipeline(req_other);
    auto rc = engine.construct_pipeline(request);
    auto rd = engine.construct_pipeline(req_other);
    
    assert(ra == rc); // Same request = same result
    assert(rb == rd); // Order doesn't matter
    
    return 0;
}