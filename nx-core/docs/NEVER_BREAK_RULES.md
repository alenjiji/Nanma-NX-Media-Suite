# NEVER BREAK RULES — NX-Core

THIS DOCUMENT IS LAW.
VIOLATIONS ARE ARCHITECTURAL FAILURES.

---

## ❌ NEVER DO (HARD STOPS)

- Introduce wall-clock time
- Introduce system clocks
- Introduce randomness
- Introduce exceptions for control flow
- Add global mutable state
- Add static mutable state
- Add base classes to core primitives
- Add inheritance to value types
- Weaken types to "fix a build"
- Change API contracts silently
- Add logging inside NX-Core
- Add hidden side effects
- Add convenience shortcuts that bypass contracts

---

## ✅ ALWAYS DO

- Preserve determinism
- Preserve explicit inputs and outputs
- Preserve explicit ownership
- Preserve immutability at boundaries
- Preserve Result / VoidResult / MultiResult flow
- Preserve stable ordering
- Preserve serialization stability
- Prefer compile-time failure over architectural violation
- Escalate when unsure

---

## 🚨 ESCALATION RULE

IF A CHANGE:

- Makes code "easier" but less explicit
- Introduces ambiguity
- Weakens a Phase 0 invariant
- Bypasses a contract rule

👉 STOP  
👉 DO NOT MERGE  
👉 ESCALATE FOR REVIEW

FINAL RULE:
NX-Core correctness is more important than convenience, speed, or feature delivery.