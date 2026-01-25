#pragma once

namespace nx::meta {

/**
 * Engine-local error enumeration
 * Deterministic error classification
 * NO LOGIC — PHASE 1.A
 */
enum class MetaError : uint32_t {
    InvalidRequest = 1,
    RepairPlanningFailed = 2,
    DeterminismViolation = 3,
    ForbiddenOperation = 4
};

}