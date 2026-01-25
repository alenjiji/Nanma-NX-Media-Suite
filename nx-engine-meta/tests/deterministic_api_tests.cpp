// 🔒 ARCHITECTURAL DETERMINISM TEST
// Phase 1 invariant — MUST NOT be weakened.
// Modifications require architectural review.
#include <nx/meta/MetaEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::meta;

// PHASE 1.B — DETERMINISM PROOF (NO SEMANTICS)
void identical_requests_produce_identical_results() {
    std::cout << "Testing identical requests...";
    MetaEngine engine;

    MetaRepairRequest a{
        nx::core::LogicalClock{1},
        42,
        ContainerId{100},
        RepairGraphId{200}
    };

    MetaRepairRequest b = a;

    auto r1 = engine.plan_repair(a);
    auto r2 = engine.plan_repair(b);

    // Prove deterministic behavior - identical inputs = identical outputs
    assert(r1 == r2);
    std::cout << " PASS\n";
}

void engine_has_no_side_effects() {
    std::cout << "Testing no side effects...";
    MetaEngine engine;

    MetaRepairRequest request{
        nx::core::LogicalClock{1},
        42,
        ContainerId{100},
        RepairGraphId{200}
    };

    // Multiple calls should produce identical results (no side effects)
    auto r1 = engine.plan_repair(request);
    auto r2 = engine.plan_repair(request);
    auto r3 = engine.plan_repair(request);

    assert(r1 == r2);
    assert(r2 == r3);
    std::cout << " PASS\n";
}

void different_logical_clocks_produce_deterministic_results() {
    std::cout << "Testing different logical clocks...";
    MetaEngine engine;

    MetaRepairRequest a{
        nx::core::LogicalClock{1},
        42,
        ContainerId{100},
        RepairGraphId{200}
    };

    MetaRepairRequest b{
        nx::core::LogicalClock{2},
        42,
        ContainerId{100},
        RepairGraphId{200}
    };

    auto r1 = engine.plan_repair(a);
    auto r2 = engine.plan_repair(b);
    auto r3 = engine.plan_repair(a);
    auto r4 = engine.plan_repair(b);

    // Same inputs produce same outputs (determinism)
    assert(r1 == r3);
    assert(r2 == r4);
    std::cout << " PASS\n";
}

void value_type_equality() {
    std::cout << "Testing value type equality...";
    MetaRepairRequest req1{
        nx::core::LogicalClock{5},
        123,
        ContainerId{456},
        RepairGraphId{789}
    };

    MetaRepairRequest req2{
        nx::core::LogicalClock{5},
        123,
        ContainerId{456},
        RepairGraphId{789}
    };

    // Value types with same values are equal
    assert(req1 == req2);
    assert(!(req1 != req2));
    std::cout << " PASS\n";
}

int main() {
    std::cout << "NX-Meta Deterministic API Tests\n";
    identical_requests_produce_identical_results();
    engine_has_no_side_effects();
    different_logical_clocks_produce_deterministic_results();
    value_type_equality();
    std::cout << "All tests PASSED\n";
    return 0;
}