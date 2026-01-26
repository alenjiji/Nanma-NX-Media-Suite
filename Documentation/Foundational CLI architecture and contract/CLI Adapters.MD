# Phase 4 — CLI Adapters

## Architecture, Command Model, and Validation Contract

---

## 4.0 Purpose of the CLI Layer (Non-Negotiable)

The CLI layer is a **headless adapter** that:

* Exposes **existing core engine operations only**
* Performs **no orchestration beyond argument → engine mapping**
* Introduces **no defaults, heuristics, or convenience logic**
* Produces **deterministic, audit-grade executions**

The CLI is **not**:

* A workflow engine (NX-BatchFlow already owns that)
* A scripting language
* A smart assistant
* A UI replacement with shortcuts

---

## 4.1 Layer Position (Immutable)

**CLI adapters sit in:**

```
Layer 3 — API & Automation
```

### Dependency rules

* CLI → Business Logic → Core Engine
* CLI **must not**:

  * Access Core internals directly
  * Perform media analysis
  * Perform policy resolution
  * Modify engine behavior

---

## 4.2 CLI Binary Layout

Single umbrella binary:

```
nx
```

Subcommands map **1:1 to engines**:

```
nx convert     → nx::convert::TranscodeEngine
nx audio       → nx::audio::AudioEngine
nx video       → nx::video::VideoEngine
nx metafix     → nx::meta::MetaEngine
nx batch       → nx::batch::BatchEngine
nx monitor     → nx::monitor::TelemetryEngine (read-only)
```

No aliases.
No shorthand binaries.
No implicit routing.

---

## 4.3 Command Structure (Strict)

### General form

```
nx <component> <operation> [--explicit-flags-only]
```

Rules:

* `<component>` is mandatory
* `<operation>` is mandatory
* Flags **must be explicit**
* Order is fixed and parseable
* Unknown flags → **hard error**

---

## 4.4 Operation Mapping Rule (Critical)

Every CLI operation must satisfy **all**:

1. Maps to **exactly one** public engine operation
2. Has **no side effects outside that operation**
3. Does not combine multiple engine calls
4. Does not infer or synthesize parameters

If an operation would require:

* Policy decisions
* Conditional logic
* Multi-step execution

👉 **It does not belong in CLI**

---

## 4.5 Example: Convert (Structural Pattern Only)

```
nx convert transcode \
  --input <path> \
  --output <path> \
  --container <target-container> \
  --video-codec <codec|passthrough> \
  --audio-codec <codec|passthrough>
```

Notes:

* `passthrough` is **explicit**, not default
* Omitted flags are **errors**
* CLI does **not** decide lossless vs lossy
* CLI does **not** resolve policy

The engine already does that.

---

## 4.6 Argument Validation Contract

### Validation happens in **three layers**

#### 1. CLI Parse Validation

* Flag exists?
* Flag spelled correctly?
* Required flag present?
* Value syntactically valid?

Failure → exit code ≠ 0, no engine call

---

#### 2. CLI Semantic Validation (Minimal)

Allowed only:

* Enum validation (known codecs, containers, modes)
* Mutually exclusive flags
* Structural consistency

Forbidden:

* Media inspection
* Compatibility checking
* Quality decisions
* Automatic corrections

---

#### 3. Engine Validation

All real validation happens **inside engines**
CLI must **not replicate** engine logic.

---

## 4.7 Explicitness Rules (Zero Exceptions)

### Forbidden CLI behavior

* Default codecs
* Default bitrates
* Default color spaces
* Default sample rates
* Auto-enable SRC, scaling, normalization
* “If omitted, assume X”

If the engine requires a parameter:
👉 the CLI **must require it explicitly**

---

## 4.8 Determinism & Auditability

CLI must guarantee:

* Identical argv → identical engine invocation
* Stable flag names across versions
* Deterministic ordering of parameters passed to engines
* Complete echoability for audit logs

### Required CLI output modes

```
--dry-run        (prints resolved engine call, no execution)
--json           (machine-readable output only)
--no-color       (pure text)
```

No implicit logging formats.

---

## 4.9 Error Model

### Errors are classified as:

* **CLI_USAGE_ERROR**

  * Missing flags
  * Invalid enum
  * Unknown command

* **ENGINE_REJECTED**

  * Passed through verbatim from engine
  * CLI does not reinterpret

* **EXECUTION_FAILED**

  * Runtime failure after engine start

Each error must emit:

* Stable error code
* Human-readable message
* Optional JSON form

---

## 4.10 Help & Discoverability Rules

* `nx --help` lists components only
* `nx <component> --help` lists operations
* `nx <component> <operation> --help` lists flags

Help text must:

* Be static
* Be versioned
* Contain **no advice or recommendations**

---

## 4.11 What Is Explicitly Deferred

Not implemented in Phase 4:

* Interactive prompts
* Shell completion
* Preset loading (BatchFlow owns this)
* Python bindings
* Qt bindings
* Convenience wrappers

---

## 4.12 Phase 4 Deliverables

1. CLI command grammar specification
2. Argument schema definitions per component
3. Validation layer (parse + minimal semantic)
4. Engine invocation adapters
5. Deterministic output & error formatting