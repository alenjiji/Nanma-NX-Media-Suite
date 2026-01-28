# Phase 15 — Mental Model (Qt UI Development Proceedings)

Status: FROZEN (Phase 15 Architectural Contract)
Scope: Phase 15 Development Guidance
Authority: CLI Canonical Contract

**Status:** Phase 15.1 Complete & Frozen → Phase 15.2 Ready
**Architecture:** Terminal Consumer Pattern (CLI Authority Preserved)

---

## Current Position

### Phase 15.1 ✅ COMPLETE & IMMUTABLE
- Qt UI architectural constraints established
- Dependency rules frozen (CLI-only)
- Adapter pattern mandated
- Error handling invariants locked
- State ownership rules defined

### Phase 15.2 ⏳ NEXT (UI Adapter Implementation)
**Objective:** Create stateless CLI adapters as Qt-Core bridge

**Implementation Requirements:**
- Stateless adapter classes (no business state)
- CLI subprocess invocation only
- DTO parsing without semantic interpretation
- Unit testable without Qt dependencies
- Mockable CLI layer for testing

---

## Architectural Flow (Immutable)

```
User Input → Qt Widgets → UI Adapters → CLI Subprocess → Core System
                ↑              ↑            ↑
            Presentation   Stateless    Canonical
             Layer Only    Bridge      Contract
```

**Critical Path:** Qt never touches core directly. CLI remains authoritative.

---

## Phase 15.2 Implementation Strategy

### 1. CLI Adapter Foundation
Create adapter base class with:
- CLI command construction
- Subprocess execution
- Response parsing
- Error code preservation

### 2. Command-Specific Adapters
Implement adapters for:
- Batch operations (status, create, monitor)
- Policy queries (list, validate, export)
- System operations (health, version, help)

### 3. Adapter Testing
- Unit tests without Qt
- Mock CLI responses
- Error handling verification
- Golden output validation

### 4. Integration Preparation
- Adapter interface contracts
- Qt widget integration points
- Error propagation patterns

---

## Implementation Constraints (Non-Negotiable)

### Adapter Rules
- **Stateless:** No persistent business state
- **Thin:** Maximum CLI delegation
- **Isolated:** One adapter per CLI command group
- **Testable:** Qt-free unit testing
- **Mockable:** CLI layer abstraction

### Forbidden Implementations
- Direct Phase 12 calls
- Business logic in adapters
- Default parameter inference
- Error message "improvement"
- State caching beyond UI presentation

### Success Criteria
- Adapters compile without core headers
- CLI contract compliance verified
- Error semantics preserved exactly
- UI deletion test passes (system unchanged)

---

## Next Phase Readiness

**Phase 15.2 Prerequisites Met:**
- Architecture constraints frozen ✅
- Dependency rules established ✅
- Adapter pattern defined ✅
- Testing strategy confirmed ✅

**Ready to implement UI adapters under architectural constraints.**

---

## Enforcement Reminders

If any implementation would:
- Call Phase 12 interpretation engine
- Add UI-specific defaults or logic
- Modify CLI error semantics
- Cache authoritative state

**→ STOP, REPORT VIOLATION, DO NOT IMPLEMENT WORKAROUND**

**CLI authority is sacred. Qt UI is terminal consumer only.**