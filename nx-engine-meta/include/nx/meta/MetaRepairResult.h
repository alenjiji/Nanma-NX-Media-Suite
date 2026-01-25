#pragma once
#include <result.h>
#include "MetaTypes.h"

namespace nx::meta {

/**
 * Deterministic outcome of repair planning or validation
 * NOT repaired files
 * NOT runtime execution results
 * 
 * NO LOGIC — PHASE 1.A
 * Compile-only skeleton
 * Media essence is read-only by default
 * Deterministic by construction
 */
struct MetaRepairOutcome final {
    RepairGraphId repair_graph_id;
    uint64_t validation_token;

    constexpr bool operator==(const MetaRepairOutcome&) const = default;
    constexpr bool operator!=(const MetaRepairOutcome&) const = default;
};

/**
 * Result type for metadata repair operations
 * Uses nx-core Result system for deterministic error handling
 */
using MetaRepairResult = nx::core::Result<MetaRepairOutcome>;

}