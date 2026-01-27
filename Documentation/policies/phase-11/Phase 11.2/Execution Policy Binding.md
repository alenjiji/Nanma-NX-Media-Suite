# Phase 11.2 — Execution Policy Binding Model (FROZEN)

⚠️ CONTRACT NOTICE ⚠️

This document defines a PURE DATA BINDING MODEL ONLY.

This phase explicitly forbids:
- Policy evaluation
- Policy resolution
- Policy merging
- Policy inheritance
- Policy overrides
- Policy precedence rules
- Runtime interpretation
- Scheduling or execution logic
- Retry or recovery semantics
- Defaults or inferred behavior

Phase 11.2 defines **association only**.

ExecutionPolicyBinding is declarative and inert.
All meaning belongs to later phases.

This document is IMMUTABLE once accepted.

---

## 0. Phase Dependency

Phase 11.2 depends on **Phase 11.1 — Execution Policy Model**.

- Phase 11.1 defines *what a policy is*
- Phase 11.2 defines *where a policy is attached*

This phase does not modify, extend, or reinterpret Phase 11.1.

---

## 1. Purpose & Scope

### What This Phase IS

- A pure association layer between execution subjects and policy identifiers
- Data-only
- Immutable
- Serializable
- Hashable
- Deterministic

### What This Phase IS NOT

- ❌ No execution semantics
- ❌ No validation of policy meaning
- ❌ No conflict detection or resolution
- ❌ No policy activation logic
- ❌ No hierarchy or inheritance
- ❌ No runtime behavior

If a concept answers *“which policy applies”*, it does not belong here.

---

## 2. Core Concept: Policy Binding

A **policy binding** answers exactly one question:

> “Which ExecutionPolicy IDs are explicitly attached to this subject?”

It does **not** answer:
- Which policy wins
- Whether policies conflict
- Whether policies are compatible
- How policies are enforced

---

## 3. Binding Subjects (Closed Set)

Only the following subjects may participate in policy binding:

```cpp
enum class PolicySubjectKind {
    Job,
    JobGraph,
    Artifact,
    BatchSession
};
````

This set is **closed** for Phase 11.x.

Any extension requires a new phase.

---

## 4. Canonical Data Model

### 4.1 ExecutionPolicyBinding

```cpp
struct ExecutionPolicyBinding {
    BindingId                id;
    PolicySubject            subject;
    std::vector<PolicyId>    attached_policies;
    BindingMetadata          metadata;
};
```

---

### 4.2 PolicySubject

```cpp
struct PolicySubject {
    PolicySubjectKind kind;
    SubjectId         id;
};
```

Rules:

* `SubjectId` is opaque
* No structure is assumed
* No hierarchy is implied

---

## 5. Binding Semantics (Strict)

### 5.1 Attachment Rules

* Policies are attached **by ID only**
* Order of `attached_policies` has **no semantic meaning**
* Duplicate policy IDs are invalid
* An empty policy list is invalid

---

### 5.2 No Inheritance Rule

Phase 11.2 explicitly forbids:

* Parent → child inheritance
* Graph → job propagation
* Session → job overrides
* Implicit policy carry-over

Bindings are **local and explicit** only.

---

### 5.3 No Conflict Resolution

Phase 11.2:

* Does NOT detect conflicts
* Does NOT resolve conflicts
* Does NOT reject conflicting policies

Conflicts are allowed to exist as **data**.

---

## 6. Identity, Hashing & Immutability

### 6.1 Binding Identity

```cpp
using BindingId = ContentHash;
```

The binding ID is computed from:

* Subject kind
* Subject ID
* Sorted list of policy IDs

Excluded from hashing:

* Metadata
* Descriptions
* Timestamps

---

### 6.2 Immutability Rules

* Bindings are immutable
* Any change requires a new binding
* No partial updates
* No runtime mutation

---

## 7. Validation Rules (Static Only)

A binding is invalid if:

* Subject kind is unknown
* Subject ID is empty
* No policies are attached
* Policy IDs are duplicated

Optional (strict mode):

* Policy IDs must resolve to known ExecutionPolicy records

Validation must be:

* Deterministic
* Side-effect free
* Non-interpreting

---

## 8. Explicit Non-Goals (Hard Line)

Phase 11.2 explicitly rejects:

* ❌ Policy priority
* ❌ Policy precedence
* ❌ Policy merging
* ❌ Overrides
* ❌ “Effective policy” computation
* ❌ Default or fallback policies
* ❌ Contextual or conditional binding
* ❌ Runtime policy activation

All such concepts require interpretation and belong to later phases.

---

## 9. Architectural Positioning

```
[ Job / Graph / Artifact / Session ]
                │
                ▼
[ ExecutionPolicyBinding ]   ← Phase 11.2
                │
                ▼
[ ExecutionPolicy ]          ← Phase 11.1
                │
                ▼
[ Interpreter / Engine ]     ← Phase 12+
```

Key invariant:

**Bindings reference policies.
Policies never reference bindings.**

---

## 10. Phase Completion Criteria

Phase 11.2 is complete when:

* Bindings are pure data
* No evaluation logic exists
* No runtime engine references appear
* Serialization is deterministic

Any code that:

* Interprets
* Enforces
* Resolves

…violates this phase.

---

## 11. Readiness for Next Phase

Only after Phase 11.2 is frozen may the system introduce:

**Phase 12 — Policy Interpretation Engine**

That phase may:

* Read policies and bindings
* Compute meaning
* Make execution decisions

But must still never mutate Phase 11.1 or 11.2 data.

---

## 12. Usage Guidance for Automation Tools

When used with automation or LLM tools:

Allowed tasks:

* Data model generation
* Schema validation
* Serialization design
* Static audits

Forbidden tasks:

* Behavioral explanation
* Execution logic
* Conflict resolution
* Scheduling advice

Violations must be rejected.

---