#include <nx/video/VideoEngine.h>
#include <cassert>

using namespace nx::video;

int main() {
    VideoEngine engine;
    
    VideoProcessRequest request{
        LogicalClock{1},
        42,
        100,
        200
    };
    
    auto r1 = engine.construct_pipeline(request);
    auto r2 = engine.construct_pipeline(request);
    assert(r1 == r2);
    
    return 0;
}