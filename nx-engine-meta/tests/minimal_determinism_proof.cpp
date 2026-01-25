// 🔒 ARCHITECTURAL DETERMINISM TEST
// Phase 1 invariant — MUST NOT be weakened.
// Modifications require architectural review.
#include <nx/meta/MetaEngine.h>
#include <cassert>
#include <iostream>

using namespace nx::meta;

// PHASE 1.B — MINIMAL DETERMINISM PROOF (NO SEMANTICS)
int main() {
    std::cout << "NX-Meta Minimal Determinism Proof\n";
    MetaEngine engine;
    
    MetaRepairRequest req{
        nx::core::LogicalClock{1},
        42,
        ContainerId{100},
        RepairGraphId{200}
    };
    
    // Canonical determinism proof
    auto r1 = engine.plan_repair(req);
    auto r2 = engine.plan_repair(req);
    assert(r1 == r2);
    
    MetaEngine e2;
    auto r3 = e2.plan_repair(req);
    assert(r1 == r3);
    
    std::cout << "Determinism proof: PASS\n";
    return 0;
}