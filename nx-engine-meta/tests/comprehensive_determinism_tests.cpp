// 🔒 ARCHITECTURAL DETERMINISM TEST
// Phase 1 invariant — MUST NOT be weakened.
// Modifications require architectural review.
#include <nx/meta/MetaEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::meta;

// PHASE 1.B — DETERMINISM PROOF (NO SEMANTICS)
int main() {
    std::cout << "NX-Meta Comprehensive Determinism Tests\n";
    MetaEngine engine;
    
    MetaRepairRequest request{
        nx::core::LogicalClock{1},
        42,
        ContainerId{100},
        RepairGraphId{200}
    };
    
    // Proof 1: Same input → same output
    auto r1 = engine.plan_repair(request);
    auto r2 = engine.plan_repair(request);
    assert(r1 == r2);
    std::cout << "Same input = same output: PASS\n";
    
    // Proof 2: Cross-instance determinism
    MetaEngine engine2;
    auto r3 = engine2.plan_repair(request);
    assert(r1 == r3);
    std::cout << "Cross-instance determinism: PASS\n";
    
    // Proof 3: Order stability
    MetaRepairRequest req_other{
        nx::core::LogicalClock{2},
        43,
        ContainerId{101},
        RepairGraphId{201}
    };
    
    auto ra = engine.plan_repair(request);
    auto rb = engine.plan_repair(req_other);
    auto rc = engine.plan_repair(request);
    auto rd = engine.plan_repair(req_other);
    
    assert(ra == rc); // Same request = same result
    assert(rb == rd); // Order doesn't matter
    std::cout << "Order stability: PASS\n";
    
    // Proof 4: Replayability
    for (int i = 0; i < 5; ++i) {
        auto replay = engine.plan_repair(request);
        assert(replay == r1);
    }
    std::cout << "Replayability: PASS\n";
    
    // Proof 5: LogicalClock determinism
    MetaRepairRequest clock_test1{
        nx::core::LogicalClock{10},
        100,
        ContainerId{500},
        RepairGraphId{600}
    };
    
    MetaRepairRequest clock_test2{
        nx::core::LogicalClock{10},
        100,
        ContainerId{500},
        RepairGraphId{600}
    };
    
    auto clock_r1 = engine.plan_repair(clock_test1);
    auto clock_r2 = engine.plan_repair(clock_test2);
    assert(clock_r1 == clock_r2);
    std::cout << "LogicalClock determinism: PASS\n";
    
    std::cout << "All comprehensive tests PASSED\n";
    return 0;
}