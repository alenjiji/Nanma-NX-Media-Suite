#pragma once
#include <cstdint>

namespace nx::meta {

/**
 * Strong value types for metadata operations
 * NO LOGIC — PHASE 1.A
 */

struct ContainerId final {
    uint64_t value;
    constexpr bool operator==(const ContainerId&) const = default;
};

struct MetadataNodeId final {
    uint64_t value;
    constexpr bool operator==(const MetadataNodeId&) const = default;
};

struct RepairGraphId final {
    uint64_t value;
    constexpr bool operator==(const RepairGraphId&) const = default;
};

}