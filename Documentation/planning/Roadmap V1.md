A
---

## **PROMPT 14A.1 — Define CLI Batch Introspection Contract**

**Objective**
Define the **read-only CLI contract** for Batch Introspection in Phase 14A.
This contract specifies *what information may be exposed* via CLI, *how it is addressed*, and *what is explicitly forbidden*.

---

### **Authoritative Constraints**

* Phase 14A is **strictly read-only**
* CLI is a **lossless projection** of existing Batch artifacts
* No runtime hooks into schedulers, engines, or live state
* No mutation, replay, retry, resume, or execution triggers
* Must consume only:

  * Batch plans
  * Materialized DAGs
  * Execution reports
  * Telemetry / timelines
  * Policy resolution artifacts

---

### **Define the CLI Contract**

#### 1. Command Namespace

Define a dedicated namespace for batch introspection, for example:

```
nx batch inspect ...
```

(no execution verbs allowed)

---

#### 2. Required Introspection Surfaces

**A. Batch Plan / DAG**

* List jobs
* Show dependencies
* Display execution order
* Show job identity & type
* Show resolved retry & failure policies (resolved view only)

**B. Job-Level State (Materialized Only)**

* Final state: success / failed / skipped
* Retry count (from records)
* Failure classification (if any)

**C. Artifacts**

* Reports
* Validation summaries
* Hashes
* Timelines
* Logs (read-only, structured)

**D. Policy Resolution Visibility**

* Which policy applied
* Final resolved decision
* No policy re-evaluation

---

#### 3. Addressing Model

Specify how targets are addressed:

* Batch ID
* Job ID
* Optional artifact selectors

No implicit defaults, no live discovery.

---

#### 4. Output Guarantees

* Deterministic
* Stable ordering
* Machine-readable first (JSON), human-readable optional
* No truncation without explicit flags

---

#### 5. Explicitly Forbidden

* Starting, stopping, retrying, or resuming batches
* Live scheduler or worker inspection
* Mutation of artifacts
* Inference or recomputation
* Any “smart” or adaptive behavior

---

### **Deliverable**

Produce:

* A **formal CLI contract**
* Enumerated commands + flags
* Input/output schema descriptions
* Invariants & non-goals section

No implementation.
No code.
No assumptions beyond existing phases.

---
