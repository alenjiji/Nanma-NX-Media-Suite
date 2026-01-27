# 🖥️ Phase 13 — CLI Contract

## **Authoritative Interface Specification**

**Status:** LOCKED · CANONICAL
**Consumes:** Phase 12 Interpretation Artifacts (READ-ONLY)
**Produces:** Serialized, auditable representations
**Does NOT:** Interpret, decide, default, execute, schedule

---

## 0. Prime Directive (Re-stated)

> **The CLI is a lossless, read-only renderer of Phase 12 artifacts.**

If the CLI "feels smart," it is wrong.

---

## 1. CLI Scope & Non-Scope

### In Scope

* Invocation of Phase 12 interpretation
* Selection of input policy sources
* Deterministic serialization of interpretation artifacts
* Machine-readable output
* Audit-friendly human-readable output (optional formatting only)

### Explicitly Out of Scope

* Policy editing
* Policy inference
* Execution control
* Scheduling
* Retries
* Defaults
* Suggestions
* UI workflows

---

## 2. Command Taxonomy (Minimal & Complete)

### 2.1 Top-Level Command

```
nx policy
```

All Phase 13 functionality is namespaced under `policy`.
No other namespaces are allowed in this phase.

---

### 2.2 Subcommands (Exhaustive)

| Subcommand  | Purpose                        |
| ----------- | ------------------------------ |
| `interpret` | Run Phase 12 interpretation    |
| `inspect`   | Display raw Phase 12 artifacts |
| `schema`    | Emit schemas for artifacts     |
| `version`   | Emit contract + phase versions |

No additional subcommands may be added without a new roadmap version.

---

## 3. `interpret` Command

### Purpose

Invoke Phase 12 interpretation and emit **interpretation artifacts**.

### Syntax

```
nx policy interpret \
  --policy <path> \
  [--output <format>] \
  [--emit <artifact-set>]
```

### Required Flags

* `--policy <path>`

  * Path to Phase 11 policy source
  * CLI performs **no validation beyond readability**

### Optional Flags (Strictly Non-Semantic)

* `--output json|yaml`

  * Serialization format only
* `--emit full|minimal`

  * Controls **volume**, not **meaning**
  * `minimal` may omit redundant echoes but never semantic fields

### Forbidden Flags

* `--fix`
* `--auto`
* `--default`
* `--force`
* `--run`
* `--execute`

If it sounds helpful, it's forbidden.

---

## 4. Output Contract (Core)

### 4.1 Output is the Product

> Exit codes are secondary.
> **The output artifact is the authority.**

### Required Properties

* Fully explicit
* Deterministically ordered
* Schema-valid
* Self-describing
* Traceable to Phase 11 inputs

---

### 4.2 Exit Codes (Non-Semantic)

| Code | Meaning                       |
| ---- | ----------------------------- |
| `0`  | Interpretation completed      |
| `1`  | Invocation failure (IO, args) |
| `2`  | Internal invariant violation  |

**Important:**
Policy rejection **must not** map to a non-zero exit code.

Rejection is data, not failure.

---

## 5. Rejection & Constraint Handling

### CLI MUST:

* Emit rejection states verbatim
* Include full rejection reasons
* Preserve causal chains

### CLI MUST NOT:

* Reword rejection
* Summarize rejection
* Downgrade rejection to warnings
* Suggest remediation

The CLI reports facts, not opinions.

---

## 6. Structural Mapping Rules

### 6.1 Isomorphism

CLI output structures must be **structurally isomorphic** to Phase 12 artifacts.

* Same hierarchy
* Same field names
* Same cardinality
* Same ordering rules

Flattening is forbidden unless Phase 12 already flattened it.

---

### 6.2 Naming Rules

* Field names are identical to Phase 12
* No aliases without lossless duplication
* No "pretty" renames

Human convenience never beats auditability.

---

## 7. Determinism Guarantees

CLI MUST guarantee:

* Stable ordering across runs
* Stable formatting rules
* No timestamps
* No environment-specific data
* No locale influence

Pretty-printing may add whitespace only.

---

## 8. `inspect` Command

### Purpose

Display **previously generated** Phase 12 artifacts.

### Syntax

```
nx policy inspect --artifact <path>
```

### Rules

* No re-interpretation
* No validation
* No enrichment

This command proves round-trip safety.

---

## 9. `schema` Command

### Purpose

Emit machine-readable schemas for all Phase 12 artifacts.

### Syntax

```
nx policy schema --format json
```

### Guarantees

* Versioned
* Deterministic
* Backward-compatible within roadmap

Schemas are contracts, not suggestions.

---

## 10. Versioning & Identification

### `version` Command

```
nx policy version
```

Must emit:

* Roadmap version (`v1.0 FINAL`)
* Phase number
* Contract hash
* Artifact schema versions

This enables forensic reproducibility.

---

## 11. Testing Requirements (CLI-Specific)

CLI tests MUST prove:

* Zero semantic logic
* Full artifact surface coverage
* Deterministic output
* No data loss
* No CLI-only behavior

Any test asserting "expected behavior" is invalid.
Only **expected structure and content** may be asserted.

---

## 12. Forward Compatibility Rule

Phase 13 must be designed such that:

* Phase 14 (Python) is a direct wrapper
* Phase 15 (Qt UI) is a presentation-only layer

If Python or Qt requires new semantics, Phase 13 failed.

---