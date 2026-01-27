# Nanma NX-MediaSuite — Official Roadmap (v1.1)

> **Status:** Authoritative
>
> This roadmap defines the *contractual architecture progression* of Nanma NX-MediaSuite.
> It is not aspirational and not marketing-driven. Each phase represents a frozen
> engineering boundary once completed.
>
> **Important Note (v1.1):**
> Phase 10 was refined during implementation to clearly separate deterministic replay
> guarantees from scheduling and policy concerns. This update clarifies scope without
> altering or reopening any completed phase.

---

## 🎯 Design Objectives (Global)

Nanma NX-MediaSuite is designed to be:

- Deterministic by construction
- Replayable and auditable
- Lossless-first
- Explicit in behavior
- Free of implicit state, time, and randomness

Once a phase is marked **Frozen**, its contracts are immutable.

---

## 🧱 Phase Group A — Foundation (Frozen)

### Phases 0–4 — CLI & Grammar Foundation

**Status:** ✅ Complete & Frozen

**Goals:**
- Deterministic CLI grammar
- Stable flags and outputs
- Explicit error codes
- Golden-output CI validation

**Non-Goals:**
- Execution logic
- Job lifecycle
- Persistence

---

## 🔍 Phase Group B — Observability (Frozen)

### Phase 5 — Monitor Engine

**Status:** ✅ Complete & Frozen

**Goals:**
- Read-only system visibility
- Deterministic snapshots
- No mutation, no polling

**Invariants:**
- Monitor never owns lifecycle
- Monitor never influences execution

---

## 📦 Phase Group C — Planning (Frozen)

### Phase 6 — Batch Engine (Planning Only)

**Status:** ✅ Complete & Frozen

**Goals:**
- Immutable batch planning
- Deterministic job ordering
- Stateless planning API

**Explicit Non-Goals:**
- Execution
- Scheduling
- Persistence

---

### Phase 7 — BatchPlan Session Architecture

**Status:** ✅ Complete & Frozen

**Goals:**
- Session-scoped job identity
- Immutable job sets
- Explicit lifecycle ownership

**Invariants:**
- Jobs exist without execution
- Identity is deterministic

---

## ⚙️ Phase Group D — Execution (Frozen)

### Phase 8 — Execution Engine (Synchronous)

**Status:** ✅ Complete & Frozen

**Goals:**
- Deterministic, blocking execution
- Explicit start/stop semantics

**Constraints:**
- Single-threaded
- No parallelism
- No scheduling

---

### Phase 9 — Job Lifecycle & State Model

**Status:** ✅ Complete & Frozen

**Goals:**
- Explicit job state machine
- Deterministic transitions
- Monitor-visible state

**Outcome:**
> System becomes functionally complete (v1 capability)

---

## 🔁 Phase Group E — Determinism Hardening (Frozen)

### Phase 10 — Deterministic Retries & Replay

**Status:** ✅ Complete & Frozen

**Purpose:**
Seal deterministic execution by enabling explicit retries and replay-only
persistence, without introducing recovery or autonomy.

#### Phase 10.1 — Deterministic Retry Engine

- Explicit retry execution only
- Immutable execution intent
- New `SessionJobId` per attempt
- No retry policies or backoff

#### Phase 10.2 — Execution Persistence for Replay

- Append-only persistence
- Records completed execution facts only
- Persistence is sufficient for replay
- Persistence is insufficient for resume

#### Phase 10.3 — Deterministic Replay Driver & Verification

- Offline replay of persisted executions
- Structural determinism verification
- Explicit divergence reporting
- No runtime integration

**Explicit Non-Goals (Phase 10):**
- Scheduling
- Recovery or resume
- Checkpointing
- Time-based behavior
- Autonomous retries

---

## ⏭ Phase Group F — Orchestration & Recovery (Future)

### Phase 11 — Scheduling, Policies & Recovery

**Status:** ⏳ Not Started

**Purpose:**
Introduce execution policies *on top of frozen Phase 10 contracts*.

**Planned Capabilities:**
- Scheduling strategies
- Retry policies
- Concurrency control
- Resume and recovery semantics

**Constraint:**
- Must not modify Phase 10 behavior

---

### Phase 12 — Parallelism & Performance

**Status:** ⏳ Not Started

**Purpose:**
Scale execution performance without violating determinism.

**Planned Capabilities:**
- Worker pools
- Resource-aware execution
- Deterministic parallelism

---

## 🧠 Permanent Architectural Invariants

These invariants apply to *all phases*:

- Determinism over convenience
- Explicit over implicit behavior
- Replay explains the past; never controls the future
- Persistence never enables resume unless explicitly designed
- UI layers are adapters only

---

## 📝 Roadmap Change Log

### v1.1
- Clarified Phase 10 as deterministic retries and replay
- Added Phase 10.3 replay verification harness
- Deferred scheduling and recovery to Phase 11
- No changes to Phases 0–9

---

> **This roadmap is now aligned with the frozen Phase 10 implementation.**
> Any future changes require a new roadmap version.