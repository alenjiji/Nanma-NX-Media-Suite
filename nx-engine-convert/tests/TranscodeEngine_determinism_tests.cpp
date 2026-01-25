#include <nx/convert/TranscodeEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::convert;

// PHASE 1.A — Deterministic API Definition Tests
void identical_requests_produce_identical_results() {
    std::cout << "Testing identical requests...";
    TranscodeEngine engine;

    TranscodeRequest a{
        LogicalClock{1},
        42,
        100,
        200
    };

    TranscodeRequest b = a;

    auto r1 = engine.prepare(a);
    auto r2 = engine.prepare(b);

    // Prove deterministic behavior - identical inputs = identical outputs
    assert(r1 == r2);
    std::cout << " PASS\n";
}

void different_logical_clocks_produce_same_deterministic_failure() {
    std::cout << "Testing different logical clocks...";
    TranscodeEngine engine;

    TranscodeRequest a{
        LogicalClock{1},
        42,
        100,
        200
    };

    TranscodeRequest b{
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

void engine_has_no_side_effects() {
    std::cout << "Testing no side effects...";
    TranscodeEngine engine;

    TranscodeRequest request{
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

int main() {
    std::cout << "NX-Convert Determinism Tests\n";
    identical_requests_produce_identical_results();
    different_logical_clocks_produce_same_deterministic_failure();
    engine_has_no_side_effects();
    std::cout << "All tests PASSED\n";
    return 0;
}
