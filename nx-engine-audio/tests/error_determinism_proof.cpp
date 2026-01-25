#include <nx/audio/AudioEngine.h>
#include <cassert>

using namespace nx::audio;

int main() {
    AudioEngine engine1;
    AudioEngine engine2;
    
    // Test identical invalid requests
    AudioRequest invalid_req{
        nx::core::LogicalClock{1},
        0, // invalid ID
        0, // invalid audio
        0  // invalid format
    };
    
    // Error equality test - same engine
    auto error1a = engine1.prepare(invalid_req);
    auto error1b = engine1.prepare(invalid_req);
    assert(error1a == error1b);
    
    // Error equality test - cross-instance
    auto error2 = engine2.prepare(invalid_req);
    assert(error1a == error2);
    
    // Error stability across runs (N=3)
    auto e1 = engine1.prepare(invalid_req);
    auto e2 = engine1.prepare(invalid_req);
    auto e3 = engine1.prepare(invalid_req);
    
    assert(e1 == e2);
    assert(e2 == e3);
    
    // Different invalid requests produce consistent errors
    AudioRequest invalid_req2{
        nx::core::LogicalClock{2},
        0, 0, 0
    };
    
    auto err_a1 = engine1.prepare(invalid_req2);
    auto err_a2 = engine1.prepare(invalid_req2);
    assert(err_a1 == err_a2);
    
    return 0;
}