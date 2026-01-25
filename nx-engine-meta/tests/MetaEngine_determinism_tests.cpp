#include <nx/meta/MetaEngine.h>
#include <cassert>

using namespace nx::meta;

// NO LOGIC — PHASE 1.A
// Deterministic unit tests that compile
// Media essence is read-only by default
// Deterministic by construction

int main() {
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
    
    // Proof 2: No hidden time dependency (LogicalClock only)
    MetaEngine engine2;
    auto r3 = engine2.plan_repair(request);
    assert(r1 == r3);
    
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
    
    return 0;
}