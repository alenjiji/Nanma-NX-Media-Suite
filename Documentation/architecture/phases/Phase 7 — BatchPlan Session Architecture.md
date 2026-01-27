Perfect call.
Phase 7 is **pure architecture**, so this must be specified *completely* before a single line of code exists.

Below is a **formal, implementation-binding specification**. Treat this as a contract that future code is not allowed to violate.

---

# 📘 Phase 7 — BatchPlan Session Architecture

**(Formal Contract Specification)**

---

## 1️⃣ Phase Intent (Non-Negotiable)

**Phase 7 introduces job identity and lifecycle ownership without execution.**

This phase exists to answer **one question only**:

> *“Where do jobs live, and who owns their state?”*

It does **NOT** answer:

* how jobs execute
* when jobs execute
* how long jobs take
* whether jobs succeed or fail

Those belong to later phases.

---

## 2️⃣ New Core Concept: `BatchPlanSession`

### Definition

A **BatchPlanSession** is an **immutable snapshot** of a planned batch, bound to a unique session identity.

It represents:

* a *single planning event*
* a *fixed set of jobs*
* a *stable job namespace*

---

## 3️⃣ Ownership Model (Critical)

| Entity                  | Owns What          |
| ----------------------- | ------------------ |
| CLI                     | ❌ Nothing          |
| BatchEngine             | ❌ Nothing          |
| MonitorEngine           | ❌ Nothing          |
| **BatchPlanSession**    | ✅ Jobs, job states |
| ExecutionEngine (later) | ❌ (reads only)     |

👉 **Only the session owns job state.**

---

## 4️⃣ Session Identity

### `SessionId`

```cpp
struct SessionId {
    std::string value;   // e.g. "session-2026-01-30-001"
};
```

### Rules

* Generated deterministically
* Unique per planning call
* Never reused
* Never inferred from file path
* Never implicit

---

## 5️⃣ Job Identity (Within a Session)

### `SessionJobId`

```cpp
struct SessionJobId {
    SessionId session;
    JobId job;
};
```

### Rules

* `job-001` only has meaning *inside* a session
* Job IDs **never collide across sessions**
* Monitor must always report jobs **with session context**

---

## 6️⃣ Job State Model (Phase 7 Scope)

### Allowed States

```cpp
enum class JobState {
    Planned,
    Rejected
};
```

### Forbidden States (NOT YET)

❌ Running
❌ Completed
❌ Failed
❌ Cancelled

These are **explicitly illegal** until Phase 8/9.

---

## 7️⃣ BatchPlanSession Contract

### Interface (Conceptual)

```cpp
class BatchPlanSession {
public:
    SessionId id() const;

    std::vector<BatchJobSummary> jobs() const;
    std::optional<BatchJobDetail> job(const JobId&) const;

    BatchPlan snapshot() const;
};
```

### Invariants

* Session is **immutable**
* No mutation after creation
* Repeated calls are deterministic
* Thread-safe by construction (no mutation)

---

## 8️⃣ BatchEngine Responsibilities (Revised)

### BatchEngine now does:

✔ Create `BatchPlanSession`
✔ Populate job metadata
✔ Assign job IDs
✔ Validate commands

### BatchEngine still does NOT:

❌ Execute jobs
❌ Track progress
❌ Store global state
❌ Maintain registries

---

## 9️⃣ MonitorEngine Integration Rules

### Monitor MAY:

* List sessions
* List jobs **within sessions**
* Read job states
* Aggregate counts across sessions

### Monitor MUST NOT:

* Create sessions
* Mutate sessions
* Infer job state
* Trigger planning

Monitor is a **read-only observer**.

---

## 🔟 Observability Model

Monitor views the world as:

```
System
 ├─ Session A
 │   ├─ job-001 (Planned)
 │   ├─ job-002 (Rejected)
 ├─ Session B
 │   ├─ job-001 (Planned)
```

👉 No “global job list” without session context is allowed.

---

## 1️⃣1️⃣ Determinism Rules

Phase 7 **must satisfy**:

* Same input → same session contents
* Session creation order is explicit
* No timestamps
* No clocks
* No randomness
* No side effects

---

## 1️⃣2️⃣ What Phase 7 Explicitly Does NOT Contain

🚫 Execution
🚫 Scheduling
🚫 Threads
🚫 Async
🚫 Persistence
🚫 Recovery
🚫 CLI changes

If any of these appear, Phase 7 is violated.

---

## 1️⃣3️⃣ Testability Contract

Every implementation must prove:

* Session immutability
* Stable job IDs
* Independent sessions
* Monitor visibility without mutation
* Snapshot consistency

No mocks. No timing hacks.

---

## 1️⃣4️⃣ Completion Definition (Phase 7)

Phase 7 is **complete** when:

✔ Sessions exist
✔ Jobs have stable identity
✔ Monitor can observe sessions
✔ No execution logic exists

At this point:

> **The system can describe work perfectly, but cannot perform it.**

This is intentional.

---

## 1️⃣5️⃣ Architectural Boundary (Hard Stop)

🚧 **Execution logic is ILLEGAL before Phase 8.**

Phase 7 ends here.

---

## ✅ Phase 7 Summary

| Aspect             | Status      |
| ------------------ | ----------- |
| Job identity       | ✔ Defined   |
| Ownership          | ✔ Explicit  |
| Monitor visibility | ✔ Read-only |
| Execution          | ❌ Forbidden |
| Mutation           | ❌ Forbidden |

---