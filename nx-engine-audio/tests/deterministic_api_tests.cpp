// 🔒 ARCHITECTURAL DETERMINISM TEST
// Phase 1 invariant — MUST NOT be weakened.
// Modifications require architectural review.
#include <nx/audio/AudioEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::audio;

// PHASE 1.A — Deterministic API Definition Tests
void identical_requests_produce_identical_results() {
    std::cout << "Testing identical requests...";
    AudioEngine engine;

    AudioRequest a{
        LogicalClock{1},
        42,
        100,
        200
    };

    AudioRequest b = a;

    auto r1 = engine.prepare(a);
    auto r2 = engine.prepare(b);

    // Prove deterministic behavior - identical inputs = identical outputs
    assert(r1 == r2);
    std::cout << " PASS\n";
}

void engine_has_no_side_effects() {
    std::cout << "Testing no side effects...";
    AudioEngine engine;

    AudioRequest request{
        LogicalClock{1},
        42,
        100,
        200
    };

    // Multiple calls should produce identical results (no side effects)
    auto r1 = engine.prepare(request);
    auto r2 = engine.prepare(request);
    auto r3 = engine.prepare(request);

    assert(r1 == r2);
    assert(r2 == r3);
    std::cout << " PASS\n";
}

void different_logical_clocks_produce_deterministic_results() {
    std::cout << "Testing different logical clocks...";
    AudioEngine engine;

    AudioRequest a{
        LogicalClock{1},
        42,
        100,
        200
    };

    AudioRequest b{
        LogicalClock{2},
        42,
        100,
        200
    };

    auto r1 = engine.prepare(a);
    auto r2 = engine.prepare(b);

    // Different logical clocks should produce different results
    // But both fail deterministically in Phase 1.A
    assert(r1.is_failure());
    assert(r2.is_failure());
    std::cout << " PASS\n";
}

void value_type_equality() {
    std::cout << "Testing value type equality...";
    AudioRequest req1{
        LogicalClock{5},
        123,
        456,
        789
    };

    AudioRequest req2{
        LogicalClock{5},
        123,
        456,
        789
    };

    // Value types with same values are equal
    assert(req1 == req2);
    assert(!(req1 != req2));
    std::cout << " PASS\n";
}

int main() {
    std::cout << "NX-Audio Deterministic API Tests\n";
    identical_requests_produce_identical_results();
    engine_has_no_side_effects();
    different_logical_clocks_produce_deterministic_results();
    value_type_equality();
    std::cout << "All tests PASSED\n";
    return 0;
}