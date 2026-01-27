# Phase 11.3 — Execution Policy Compatibility Declaration (FROZEN)

⚠️ CONTRACT NOTICE ⚠️

This document defines a PURE DATA COMPATIBILITY MODEL ONLY.

This phase explicitly forbids:
- Policy evaluation
- Policy resolution
- Policy precedence
- Policy merging
- Policy overrides
- Runtime decision-making
- Scheduling or execution logic
- Retry, recovery, or fallback semantics
- Automatic compatibility inference

Phase 11.3 defines **declared compatibility only**.

All interpretation and enforcement belong to later phases.

This document is IMMUTABLE once accepted.

---

## 0. Phase Dependency

Phase 11.3 depends on:

- **Phase 11.1 — Execution Policy Model**
- **Phase 11.2 — Execution Policy Binding Model**

This phase does not modify or reinterpret earlier phases.
It only introduces **explicit compatibility declarations** between policies.

---

## 1. Purpose & Scope

### What This Phase IS

- A declarative compatibility layer between ExecutionPolicy records
- Data-only
- Immutable
- Serializable
- Hashable
- Deterministic

### What This Phase IS NOT

- ❌ No conflict resolution
- ❌ No automatic compatibility inference
- ❌ No priority or precedence rules
- ❌ No “winning” or “effective” policy computation
- ❌ No runtime behavior

This phase answers only:

> “Are these policies explicitly declared as compatible or incompatible?”

---

## 2. Core Concept: Policy Compatibility

Policy compatibility is an **explicit declaration**, not a derived property.

Compatibility:
- Is not inferred
- Is not symmetric unless declared
- Has no automatic fallback behavior

Absence of a declaration means **unknown compatibility**, not compatible.

---

## 3. Compatibility Relation Model

### 3.1 CompatibilityKind

```cpp
enum class CompatibilityKind {
    Compatible,
    Incompatible
};
````

No other states are allowed.

---

### 3.2 ExecutionPolicyCompatibility

```cpp
struct ExecutionPolicyCompatibility {
    CompatibilityId    id;
    PolicyId           left_policy;
    PolicyId           right_policy;
    CompatibilityKind  kind;
    CompatibilityMetadata metadata;
};
```

---

## 4. Compatibility Semantics (Strict)

### 4.1 Directionality

Compatibility declarations are **directional**.

* `A → B` is independent from `B → A`
* Symmetry must be declared explicitly if desired

---

### 4.2 No Transitivity Rule

Compatibility is **not transitive**.

If:

* A is compatible with B
* B is compatible with C

Nothing may be inferred about A and C.

---

### 4.3 No Default Compatibility

* Lack of a declaration means **unknown**
* Unknown is neither compatible nor incompatible
* Engines must not guess

---

## 5. Identity, Hashing & Immutability

### 5.1 Compatibility Identity

```cpp
using CompatibilityId = ContentHash;
```

Computed from:

* Left policy ID
* Right policy ID
* Compatibility kind

Excluded from hashing:

* Metadata
* Descriptions
* Timestamps

---

### 5.2 Immutability Rules

* Compatibility records are immutable
* Any change requires a new record
* No runtime mutation
* No partial updates

---

## 6. Validation Rules (Static Only)

A compatibility record is invalid if:

* Either policy ID is missing
* Both policy IDs are identical
* Compatibility kind is unspecified
* The same compatibility record already exists

Optional (strict mode):

* Both policy IDs must resolve to known ExecutionPolicy records

Validation must be:

* Deterministic
* Side-effect free
* Non-interpreting

---

## 7. Explicit Non-Goals (Hard Line)

Phase 11.3 explicitly rejects:

* ❌ Automatic compatibility checks
* ❌ Constraint solving
* ❌ Policy graphs
* ❌ Conflict detection
* ❌ Conflict resolution
* ❌ Policy ranking
* ❌ Runtime rejection or acceptance
* ❌ “Most compatible” selection

All such behavior belongs to interpretation phases.

---

## 8. Architectural Positioning

```
[ ExecutionPolicy ]           ← Phase 11.1
        │
        ▼
[ ExecutionPolicyBinding ]    ← Phase 11.2
        │
        ▼
[ Policy Compatibility ]      ← Phase 11.3 (THIS)
        │
        ▼
[ Interpreter / Engine ]      ← Phase 12+
```

Key invariant:

**Compatibility records describe relationships.
They do not enforce outcomes.**

---

## 9. Phase Completion Criteria

Phase 11.3 is complete when:

* Compatibility is explicitly declared
* No inference logic exists
* No runtime references appear
* Serialization is deterministic

Any attempt to:

* Evaluate
* Enforce
* Interpret

…violates this phase.

---

## 10. Readiness for Next Phase

After Phase 11.3 is frozen, the system may introduce:

**Phase 12 — Policy Interpretation Engine**

That phase may:

* Read policies
* Read bindings
* Read compatibility declarations
* Make execution decisions

But must never mutate Phase 11.1, 11.2, or 11.3 data.

---

## 11. Usage Guidance for Automation Tools

Allowed tasks:

* Schema generation
* Static validation
* Deterministic serialization
* Audit reporting

Forbidden tasks:

* Behavioral explanation
* Execution advice
* Compatibility inference
* Conflict resolution

Violations must be rejected.

---
