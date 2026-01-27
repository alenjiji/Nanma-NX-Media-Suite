# Phase 11.1 — Execution Policy Model (FROZEN)

⚠️ CONTRACT NOTICE ⚠️

This document defines a PURE DATA MODEL ONLY.

This phase explicitly forbids:
- Scheduling logic
- Execution logic
- Retry or recovery semantics
- Defaults or inferred behavior
- Runtime decision-making
- Environment inspection
- Side effects of any kind

ExecutionPolicy is declarative intent only.
Any behavior belongs to later phases.

This document is IMMUTABLE once accepted.

Got it. Phase 10 is frozen ❄️
We'll define **Phase 11.1** as a **pure, inert data model**—no behavior, no scheduling semantics, no execution hints. This is a *policy declaration layer only*.

Below is the **authoritative contract**.

---

# Phase 11.1 — Execution Policy Model

**(Pure Data · Zero Behavior · Immutable Intent)**

> Phase 11.1 defines *what is allowed* and *under which constraints* — **not** *when*, *how*, or *if* it runs.

This phase introduces a **declarative execution policy layer** shared across all engines in **Nanma NX-MediaSuite**.

---

## 1. Purpose & Scope

### What this phase IS

* A **pure data description** of execution intent and constraints
* Serializable, hashable, versioned
* Engine-agnostic
* UI-agnostic
* Batch-agnostic

### What this phase is NOT

* ❌ No scheduling
* ❌ No execution
* ❌ No retries
* ❌ No recovery
* ❌ No side effects
* ❌ No decision logic

> If a field can influence *when* or *how often* something runs → **it does not belong here**.

---

## 2. Core Concept: ExecutionPolicy

`ExecutionPolicy` is an **immutable declaration** attached to a job, graph, or artifact.

It answers only:

> "Under which rules would execution be considered valid?"

---

## 3. Canonical Data Model

### 3.1 ExecutionPolicy (Root Object)

```cpp
struct ExecutionPolicy {
    PolicyId                 id;
    PolicyVersion            version;

    ExecutionSafetyPolicy    safety;
    ExecutionDeterminismPolicy determinism;
    ExecutionResourcePolicy  resources;
    ExecutionEnvironmentPolicy environment;
    ExecutionAuthorizationPolicy authorization;

    PolicyMetadata           metadata;
};
```

---

## 4. Sub-Policies (All Mandatory, All Passive)

---

### 4.1 ExecutionSafetyPolicy

Defines **what must never happen**.

```cpp
struct ExecutionSafetyPolicy {
    bool allow_lossy_operations;
    bool allow_media_essence_modification;
    bool allow_metadata_inference;
    bool allow_implicit_transforms;

    bool require_explicit_user_consent;
};
```

**Rules**

* No defaults
* No implied inheritance
* Absence = invalid

---

### 4.2 ExecutionDeterminismPolicy

Declares reproducibility requirements.

```cpp
struct ExecutionDeterminismPolicy {
    DeterminismLevel required_level;
    bool require_bit_identical_output;
    bool allow_hardware_variance;
    bool allow_threading_variance;
};
```

```cpp
enum class DeterminismLevel {
    BitExact,
    StructurallyIdentical,
    SemanticallyEquivalent
};
```

**Important**

* This does **not** enforce determinism
* It only declares the **minimum acceptable guarantee**

---

### 4.3 ExecutionResourcePolicy

Declares **resource boundaries**, not allocation.

```cpp
struct ExecutionResourcePolicy {
    std::optional<uint32_t> max_cpu_cores;
    std::optional<uint64_t> max_memory_bytes;
    std::optional<bool>     gpu_allowed;
    std::optional<uint64_t> max_disk_io_bytes;
};
```

**Notes**

* No priorities
* No timeouts
* No quotas
* No fairness concepts

---

### 4.4 ExecutionEnvironmentPolicy

Declares **where execution is valid**, not where it will run.

```cpp
struct ExecutionEnvironmentPolicy {
    std::vector<PlatformId> allowed_platforms;
    std::vector<ArchId>     allowed_architectures;

    bool require_headless_capability;
    bool forbid_ui_dependencies;
};
```

This enforces:

* Qt-free core invariants
* Headless batch compatibility

---

### 4.5 ExecutionAuthorizationPolicy

Declares **what level of authority is required**.

```cpp
struct ExecutionAuthorizationPolicy {
    AuthorizationLevel required_level;
    bool require_audit_trail;
};
```

```cpp
enum class AuthorizationLevel {
    System,
    Operator,
    ExplicitHumanConsent
};
```

No authentication.
No identity.
No credential handling.

---

## 5. PolicyMetadata (Non-Semantic)

```cpp
struct PolicyMetadata {
    std::string description;
    std::string author;
    std::chrono::utc_time<std::chrono::seconds> created_at;
};
```

* Informational only
* Ignored by engines
* Excluded from policy hash (see §6)

---

## 6. Identity, Hashing & Immutability

### 6.1 Policy Identity

```cpp
using PolicyId = ContentHash;
```

* Hash computed over **semantic fields only**
* Metadata excluded
* Order-independent
* Deterministic serialization required

### 6.2 Immutability Rules

* Policies are **append-only**
* Changes require a **new PolicyVersion**
* No in-place mutation
* No runtime overrides

---

## 7. Validation Rules (Static Only)

A policy is **invalid** if:

* Any sub-policy is missing
* Any boolean is unspecified
* Any enum is unset
* Any constraint contradicts architectural principles

Validation is:

* Deterministic
* Pure
* Side-effect free

---

## 8. Explicit Non-Goals (Hard Line)

Phase 11.1 explicitly rejects:

* ❌ Retry definitions
* ❌ Backoff strategies
* ❌ Priority levels
* ❌ Deadlines / SLAs
* ❌ Scheduling hints
* ❌ Failure handling
* ❌ "Preferred" behaviors
* ❌ Adaptive policies
* ❌ Environment detection

Those belong to **later phases**, layered *on top* of this model.

---

## 9. Architectural Positioning

```
[ Job Spec ]
     │
     ▼
[ ExecutionPolicy ]   ← Phase 11.1 (THIS)
     │
     ▼
[ Scheduler / Engine / Runtime ]
```

> Engines **consume** policies
> Policies **never observe** engines

---

## 10. Phase Boundary Declaration

**Phase 11.1 is complete when:**

* The model is frozen
* Serialization is deterministic
* Hashing is reproducible
* No execution code exists

No engine code is allowed to:

* Mutate
* Interpret creatively
* Fill in missing intent

---