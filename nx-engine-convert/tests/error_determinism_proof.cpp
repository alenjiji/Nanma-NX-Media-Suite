#include <nx/convert/TranscodeEngine.h>
#include <cassert>

using namespace nx::convert;

int main() {
    TranscodeEngine engine1;
    TranscodeEngine engine2;
    
    // Test identical invalid requests
    TranscodeRequest invalid_req{
        LogicalClock{1},
        InputContainerId{0}, // invalid ID
        OutputContainerId{0}, // invalid container
        TargetFormatId{0}  // invalid format
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
    TranscodeRequest invalid_req2{
        LogicalClock{2},
        InputContainerId{0},
        OutputContainerId{0},
        TargetFormatId{0}
    };
    
    auto err_a1 = engine1.prepare(invalid_req2);
    auto err_a2 = engine1.prepare(invalid_req2);
    assert(err_a1 == err_a2);
    
    return 0;
}