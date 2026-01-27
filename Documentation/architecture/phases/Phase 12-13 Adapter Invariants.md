# 🔗 Phase 12 → Phase 13 Adapter Invariants

## **Authoritative Contract**

**Status:** LOCKED
**Applies To:** All CLI adapters, current and future
**Purpose:** Guarantee zero behavior injection between interpretation and presentation

---

## 1. Prime Directive

> **The Phase 13 CLI MUST be a lossless, read-only projection of Phase 12 artifacts.**

Nothing is allowed to happen in the adapter that did not already happen in Phase 12.

---

## 2. Information Flow Invariant

### Allowed Direction

```
Phase 11 (Policy) → Phase 12 (Interpretation) → Phase 13 (CLI)
```

### Forbidden

* CLI → Phase 12 feedback
* CLI-side reinterpretation
* CLI-generated defaults
* CLI-generated policy meaning

The CLI has **no authority**.

---

## 3. Logic Containment Invariant

### Phase 12 MAY:

* Decide meaning
* Resolve bindings
* Evaluate compatibility
* Emit rejection reasons

### Phase 13 MUST NOT:

* Evaluate rules
* Resolve anything
* Decide success/failure
* Mask rejections
* Reword semantics

If a decision exists, it exists **only** in Phase 12.

---

## 4. Completeness Invariant

> Every Phase 12 artifact must be representable in the CLI **without loss**.

This includes:

* Rejected states
* Partial compatibility
* Constraint failures
* Ambiguous-but-explicit results

CLI convenience **must never reduce expressiveness**.

---

## 5. Default Prohibition Invariant

### CLI MUST NOT:

* Introduce defaults
* Fill missing values
* Assume "reasonable" behavior
* Auto-select policy paths

### If Phase 12 is silent:

* CLI is silent
* Output must reflect absence explicitly

Silence is a valid and intentional state.

---

## 6. Determinism Preservation Invariant

### Required:

* Stable output ordering
* Stable field naming
* Stable serialization
* No time-based values
* No environment-dependent formatting

### Forbidden:

* Locale-sensitive formatting
* Pretty-print shortcuts that alter meaning
* Condensed output modes that drop data

Formatting may change **appearance**, never **content**.

---

## 7. Error & Rejection Transparency Invariant

CLI behavior on rejection:

* Must expose rejection verbatim
* Must not reinterpret reason
* Must not convert rejection into warnings
* Must not auto-suggest fixes

CLI is a **messenger**, not a negotiator.

---

## 8. Structural Isomorphism Invariant

> CLI output structures must map 1:1 to Phase 12 artifact structures.

* No field merging
* No field splitting
* No renaming without lossless aliasing
* No flattening that destroys hierarchy

If Phase 12 is hierarchical, CLI remains hierarchical.

---

## 9. No Control-Surface Invariant

CLI MUST NOT introduce:

* Execution flags
* Retry switches
* Scheduling hints
* Engine selection
* Performance knobs

Phase 13 is **not an execution surface**.

---

## 10. Round-Trip Safety Invariant

A Phase 12 artifact:

* Serialized by CLI
* Parsed back by tooling

Must be:

* Bit-for-bit equivalent (modulo whitespace)
* Semantically identical
* Order-stable

This enables audit, diffing, CI, and compliance.

---

## 11. Testing Invariants

Phase 13 tests MUST assert:

* Zero logic in adapters
* Identical outputs for identical Phase 12 artifacts
* Full surface coverage
* No silent data loss
* No CLI-only behaviors

Any CLI test that "tests logic" is invalid by definition.

---

## 12. UI Contamination Firewall

This invariant exists to protect Phase 15.

> **If CLI introduces interpretation, Qt UI will inevitably inherit it.**

Therefore:

* CLI must remain aggressively boring
* CLI must feel "too explicit"
* CLI must resist usability sugar

Boring is correctness.

---