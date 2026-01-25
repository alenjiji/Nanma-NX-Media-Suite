// 🔒 ARCHITECTURAL DETERMINISM TEST
// Phase 1 invariant — MUST NOT be weakened.
// Modifications require architectural review.
#include <logical_clock.h>
#include <nx/convert/TranscodeEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::convert;

// PHASE 1.A — Determinism Proof (Compile-Only)

// Proof 1: Same input → same output
void same_input_same_output() {
    std::cout << "Testing same input → same output...";
    TranscodeEngine engine;
    
    TranscodeRequest request{
        LogicalClock{42},
        1001,
        2002,
        3003
    };
    
    auto result1 = engine.prepare(request);
    auto result2 = engine.prepare(request);
    auto result3 = engine.prepare(request);
    
    // Identical inputs must produce identical outputs
    assert(result1 == result2);
    assert(result2 == result3);
    assert(result1 == result3);
    std::cout << " PASS\n";
}

// Proof 2: No hidden time dependency
void no_hidden_time_dependency() {
    TranscodeEngine engine1;
    TranscodeEngine engine2;
    
    TranscodeRequest request{
        LogicalClock{100},
        5000,
        6000,
        7000
    };
    
    // Same request to different engine instances
    auto result1 = engine1.prepare(request);
    auto result2 = engine2.prepare(request);
    
    // Results must be identical (no wall-clock time dependency)
    assert(result1 == result2);
    
    // Multiple calls at different "times" (compile-time)
    auto result3 = engine1.prepare(request);
    auto result4 = engine2.prepare(request);
    
    assert(result1 == result3);
    assert(result2 == result4);
}

// Proof 3: Order stability
void order_stability() {
    TranscodeEngine engine;
    
    TranscodeRequest req_a{
        LogicalClock{10},
        100,
        200,
        300
    };
    
    TranscodeRequest req_b{
        LogicalClock{20},
        400,
        500,
        600
    };
    
    // Execute in order A, B
    auto result_a1 = engine.prepare(req_a);
    auto result_b1 = engine.prepare(req_b);
    
    // Execute in order B, A
    auto result_b2 = engine.prepare(req_b);
    auto result_a2 = engine.prepare(req_a);
    
    // Results must be identical regardless of execution order
    assert(result_a1 == result_a2);
    assert(result_b1 == result_b2);
}

// Proof 4: LogicalClock determinism
void logical_clock_determinism() {
    TranscodeEngine engine;
    
    TranscodeRequest req1{
        LogicalClock{1},
        42,
        100,
        200
    };
    
    TranscodeRequest req2{
        LogicalClock{1}, // Same LogicalClock
        42,
        100,
        200
    };
    
    auto result1 = engine.prepare(req1);
    auto result2 = engine.prepare(req2);
    
    // Same LogicalClock = same result
    assert(result1 == result2);
}

// Proof 5: Value type equality
void value_type_equality() {
    TranscodeRequest req1{
        LogicalClock{5},
        123,
        456,
        789
    };
    
    TranscodeRequest req2{
        LogicalClock{5},
        123,
        456,
        789
    };
    
    // Value types with same values are equal
    assert(req1 == req2);
    assert(!(req1 != req2));
}

int main() {
    std::cout << "NX-Convert Determinism Proof\n";
    same_input_same_output();
    no_hidden_time_dependency();
    order_stability();
    logical_clock_determinism();
    value_type_equality();
    std::cout << "All tests PASSED\n";
    return 0;
}

// Determinism proof complete:
// ✓ Same input → same output (no hidden state)
// ✓ No hidden time dependency (LogicalClock only)
// ✓ Order stability (execution order irrelevant)
// ✓ Compile-only (no logic required)
// ✓ Pure equality comparison (no mocks/timing)