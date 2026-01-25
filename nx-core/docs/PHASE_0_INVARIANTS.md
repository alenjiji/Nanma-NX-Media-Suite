# PHASE 0 — NX-Core Invariants (Frozen)

**⚠️ WARNING**
**Any violation of these invariants invalidates architectural guarantees of Nanma NX-MediaSuite.**

---

## 1. Determinism Invariants (Never Break)

- No wall-clock time
- No system clocks
- No randomness
- No hidden state
- No global mutable state
- Same inputs MUST produce the same outputs
- Behavior MUST be reproducible across runs, machines, and architectures
- Replayability is mandatory

---

## 2. Identity Invariants

- IDs are value types
- IDs MUST be final
- No base classes
- No inheritance
- No polymorphism
- IDs are content-derived only
- No UUIDs
- No timestamps
- No randomness
- ID types are non-interchangeable by construction

---

## 3. Error & Result Invariants

- No exceptions as control flow
- Result<T>, VoidResult, or MultiResult<T> ONLY
- Errors are deterministic values
- Errors are comparable
- Errors are serializable
- Error ordering is stable and reproducible
- Failure is explicit

---

## 4. API Contract Invariants

- Explicit inputs only
- Explicit outputs only
- No hidden inputs
- No hidden outputs
- Ownership MUST be explicit
- No raw owning pointers
- Immutability at API boundaries
- Types crossing boundaries MUST be:
  - Comparable
  - Serializable
  - Deterministic

---

## 5. NX-Core Boundary Invariants

- Media-agnostic
- Component-agnostic
- Qt is forbidden
- No engine logic
- No batch logic
- No monitoring logic
- NX-Core MUST depend on nothing above it
- Higher layers MAY depend on NX-Core

FINAL RULE:
These invariants are permanent and MUST NOT be weakened, bypassed, or reinterpreted.