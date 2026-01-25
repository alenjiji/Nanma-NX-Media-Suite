#pragma once
#include <logical_clock.h>
#include "MetaTypes.h"

namespace nx::meta {

/**
 * Intent to perform structural or metadata operations
 * NOT execution
 * NOT modification of media essence
 * NOT automatic correction
 * 
 * NO LOGIC — PHASE 1.A
 * Compile-only skeleton
 * Media essence is read-only by default
 * Deterministic by construction
 */
struct MetaRepairRequest final {
    nx::core::LogicalClock clock;
    uint64_t request_id;

    // Opaque handles only - NO LOGIC — PHASE 1.A
    ContainerId container_id;
    RepairGraphId repair_graph_id;

    constexpr bool operator==(const MetaRepairRequest&) const = default;
    constexpr bool operator!=(const MetaRepairRequest&) const = default;
};

}