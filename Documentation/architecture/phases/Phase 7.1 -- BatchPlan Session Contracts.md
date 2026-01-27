# Nanma NX-MediaSuite

## Phase 7.1 — **BatchPlan Session Contracts**

**(Authoritative Architectural Definition)**

This document defines **only** the *type-level*, *ownership*, and *immutability* contracts for **BatchPlan Sessions** inside **NX-BatchFlow**.
It is a **hard boundary**: anything not explicitly permitted here is forbidden in Phase 7.

This contract is written to align strictly with:

* Batch-first, deterministic architecture
* Zero side-effects observability
* No hidden execution semantics
* No time, clocks, randomness, or mutation
* No UI or CLI assumptions

---

## 1. Purpose of a BatchPlan Session

A **BatchPlan Session** is a **pure, immutable planning snapshot** created by `BatchEngine` from a validated BatchPlan.

It represents:

* A **frozen, deterministic view** of *what will be executed*
* A **stable identity boundary** for monitoring and inspection
* A **read-only graph of intended jobs**, not execution

A session **does not execute**, **does not schedule**, and **does not evolve**.

> **Key rule**:
> A BatchPlan Session describes *intent*, not *activity*.

---

## 2. Core Identity Types

### 2.1 `SessionId`

**Definition**

* An opaque, strongly-typed identifier
* Uniquely identifies **one immutable BatchPlan Session**
* Created **only** by `BatchEngine`

**Invariants**

* Deterministic for identical BatchPlan + parameters
* Stable for the lifetime of the session
* Never reused
* Never user-supplied
* No embedded meaning (no timestamps, counters, hashes exposed)

**Forbidden**

* Deriving behavior from SessionId
* Encoding ordering, timing, or priority
* Using SessionId as execution state

---

### 2.2 `SessionJobId`

**Definition**

* An opaque identifier representing a **job inside a specific session**
* Scoped **strictly** to one `SessionId`

**Invariants**

* Deterministically derived from:

  * SessionId
  * Logical JobNode identity in the BatchPlan
* Stable and repeatable
* Meaningless outside its session

**Forbidden**

* Reuse across sessions
* Cross-session comparison
* Inferring execution order or dependency timing

---

## 3. `BatchPlanSession` — Canonical Contract

### 3.1 Structural Definition (Conceptual)

A `BatchPlanSession` is an **immutable value object** containing:

* `SessionId`
* Frozen job graph snapshot
* Immutable per-job descriptors
* Immutable dependency topology
* Immutable metadata for observability

No field inside a session is mutable after creation.

---

### 3.2 Session Contents (Allowed)

A session **may expose read-only access** to:

* List of `SessionJobId`
* For each job:

  * Job type (Convert / Audio / Video / Meta)
  * Declared inputs
  * Declared outputs
  * Declared parameters
  * Dependency edges (SessionJobId → SessionJobId)
* Graph-level properties:

  * Total job count
  * DAG validity proof
  * Deterministic topological ordering (symbolic, not temporal)

> The ordering is **logical**, not a schedule.

---

### 3.3 Session Immutability Rules

Once constructed:

* No jobs may be added, removed, or reordered
* No parameters may change
* No dependencies may change
* No metadata may be mutated
* No execution results may be attached

Any variation requires **creating a new session**.

---

## 4. Determinism Guarantees

A BatchPlan Session MUST satisfy:

* **Plan Determinism**
  Identical BatchPlan + identical inputs → identical session graph

* **Structural Determinism**
  Job identity, dependencies, and ordering are reproducible

* **Observability Determinism**
  Monitoring the same session twice yields the same structure

Explicitly **not allowed**:

* Time-based IDs
* Random UUIDs
* Environment-dependent graph shaping
* Hardware-dependent branching

---

## 5. Ownership & Lifecycle Rules

### 5.1 Creation Ownership

Only **`BatchEngine`** may:

* Validate a BatchPlan
* Materialize a `BatchPlanSession`
* Assign `SessionId`
* Assign `SessionJobId`s

No other component may construct or mutate a session.

---

### 5.2 Lifetime Ownership

* A session is owned *logically* by BatchEngine
* A session is *observed* by Monitor
* A session is *consumed read-only* by future execution phases

Destruction is **out of scope** for Phase 7.

---

## 6. Monitor Interaction (Read-Only)

### 6.1 Observation Model

`NX-Monitor` may:

* Subscribe to session creation events
* Read session structure
* Index sessions by SessionId
* Correlate telemetry *against* SessionId later

Monitor **must never**:

* Attach state to a session
* Annotate jobs
* Mutate session metadata
* Infer execution status
* Create derived session variants

> A session is **not** a runtime object.

---

## 7. Explicitly Forbidden in Phase 7

The following are **strictly disallowed** at the contract level:

### 7.1 Execution Concepts

* Job states (queued, running, failed, complete)
* Progress
* Retries
* Failure handling
* Success/failure flags

---

### 7.2 Scheduling Concepts

* Workers
* Threads
* Queues
* Priorities
* Resource allocation
* Parallelism

---

### 7.3 Time & Variability

* Clocks
* Timestamps
* Durations
* Timeouts
* Randomness
* UUID generators

---

### 7.4 Structural Violations

* Collapsing sessions into jobs
* Treating sessions as jobs
* Nested sessions
* Session mutation
* Partial session reuse

---

### 7.5 Persistence & Side Effects

* Disk storage
* Serialization formats
* Database IDs
* Checkpoints
* Resume tokens

---

### 7.6 Monitor Violations

* Monitor-driven mutation
* Feedback loops
* Adaptive behavior
* Control-plane influence

---

## 8. Non-Goals (By Design)

Phase 7.1 **does not** define:

* How sessions execute
* How jobs run
* How failures propagate
* How retries work
* How scheduling happens
* How persistence works

Those belong to **later phases** and must treat this contract as immutable.

---

## 9. Contract Summary

* A **BatchPlanSession is immutable**
* A **Session describes intent, not execution**
* **BatchEngine creates; Monitor observes**
* **No clocks, no randomness, no mutation**
* **No hidden behavior, now or later**