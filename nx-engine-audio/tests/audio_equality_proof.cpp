#include <nx/audio/AudioEngine.h>
#include <cassert>

using namespace nx::audio;

int main() {
    // Test data - identical AudioRequest values (engine-domain IDs only)
    AudioRequest req1{
        nx::core::LogicalClock{42},
        1001,  // request_id
        2001,  // input_audio_id (opaque)
        3001   // target_format_id (opaque)
    };
    
    AudioRequest req2{
        nx::core::LogicalClock{42},
        1001,  // request_id
        2001,  // input_audio_id (opaque)
        3001   // target_format_id (opaque)
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
        9999,  // different request_id
        8888,  // different input_audio_id
        7777   // different target_format_id
    };
    
    auto a1 = engine1.prepare(req1);
    auto b1 = engine1.prepare(other);
    auto a2 = engine1.prepare(req1);
    auto b2 = engine1.prepare(other);
    
    assert(a1 == a2); // Same request = same result
    assert(b1 == b2); // Order doesn't matter
    
    // PROOF COMPLETE: Engine-domain ID equality verified
    // - No audio buffers used
    // - No samples processed
    // - No floating-point math
    // - No time-based behavior
    // - Pure opaque ID determinism
    
    return 0;
}