# Phase 14B — CLI ↔ Python Mirror Map

## Authority and Constraints

**CLI is Authoritative**: The nx-cli executable is the canonical interface. Python bindings are transport-only wrappers that invoke CLI commands via subprocess.

**No Independent API**: Python bindings provide NO additional behavior, validation, or features beyond what the CLI offers. They are pure mirrors.

**Breaking Change Warning**: Any CLI command changes (arguments, flags, output format) require corresponding Python binding updates. Python bindings will break if CLI interface changes.

## Command Mapping

### Monitor Commands

#### monitor status

**CLI Invocation:**
```bash
nx monitor status --json
```

**Python Callable:**
```python
import nx
result = nx.monitor.status()
```

**Arguments:** None (format="json" is default)

**Output:** JSON dict containing monitor engine status

**Explicit Note:** No additional behavior beyond CLI. Python function executes `nx monitor status --json` via subprocess and returns parsed JSON.

---

### Batch Introspection Commands

#### batch inspect plan

**CLI Invocation:**
```bash
nx batch inspect plan <batch_id> --format json [--jobs-only] [--dag-only]
```

**Python Callable:**
```python
import nx
result = nx.batch.inspect.plan(
    batch_id,
    format="json",      # "json" or "human"
    jobs_only=False,    # bool
    dag_only=False      # bool
)
```

**Arguments:** 1:1 mapping to CLI flags

**Output:** JSON dict containing batch plan structure

**Explicit Note:** No additional behavior beyond CLI. Python function constructs CLI arguments and executes via subprocess.

#### batch inspect jobs

**CLI Invocation:**
```bash
nx batch inspect jobs <batch_id> --format json [--filter-type <type>] [--sort <order>]
```

**Python Callable:**
```python
import nx
result = nx.batch.inspect.jobs(
    batch_id,
    format="json",           # "json" or "human"
    filter_type=None,        # optional string
    sort="execution"         # "execution", "dependency", or "id"
)
```

**Arguments:** 1:1 mapping to CLI flags

**Output:** JSON dict containing job inventory

**Explicit Note:** No additional behavior beyond CLI. Python function constructs CLI arguments and executes via subprocess.

#### batch inspect status

**CLI Invocation:**
```bash
nx batch inspect status <batch_id> --format json
```

**Python Callable:**
```python
import nx
result = nx.batch.inspect.status(batch_id, format="json")
```

**Arguments:** 1:1 mapping to CLI flags

**Output:** JSON dict containing batch execution status

**Explicit Note:** No additional behavior beyond CLI. Stub implementation - to be completed following established pattern.

#### batch inspect job

**CLI Invocation:**
```bash
nx batch inspect job <batch_id> <job_id> --format json
```

**Python Callable:**
```python
import nx
result = nx.batch.inspect.job(batch_id, job_id, format="json")
```

**Arguments:** 1:1 mapping to CLI flags

**Output:** JSON dict containing individual job details

**Explicit Note:** No additional behavior beyond CLI. Stub implementation - to be completed following established pattern.

#### batch inspect policies

**CLI Invocation:**
```bash
nx batch inspect policies <batch_id> --format json
```

**Python Callable:**
```python
import nx
result = nx.batch.inspect.policies(batch_id, format="json")
```

**Arguments:** 1:1 mapping to CLI flags

**Output:** JSON dict containing policy resolution data

**Explicit Note:** No additional behavior beyond CLI. Stub implementation - to be completed following established pattern.

#### batch inspect artifacts

**CLI Invocation:**
```bash
nx batch inspect artifacts <batch_id> --format json
```

**Python Callable:**
```python
import nx
result = nx.batch.inspect.artifacts(batch_id, format="json")
```

**Arguments:** 1:1 mapping to CLI flags

**Output:** JSON dict containing artifact inventory

**Explicit Note:** No additional behavior beyond CLI. Stub implementation - to be completed following established pattern.

#### batch inspect artifact

**CLI Invocation:**
```bash
nx batch inspect artifact <batch_id> <artifact_id> --format json
```

**Python Callable:**
```python
import nx
result = nx.batch.inspect.artifact(batch_id, artifact_id, format="json")
```

**Arguments:** 1:1 mapping to CLI flags

**Output:** JSON dict containing individual artifact details

**Explicit Note:** No additional behavior beyond CLI. Stub implementation - to be completed following established pattern.

## Implementation Pattern

All Python bindings follow identical pattern:

1. **Argument Validation:** Basic type checking only
2. **CLI Construction:** Build argument vector matching CLI syntax exactly
3. **Subprocess Execution:** Invoke resolved nx-cli executable
4. **Error Propagation:** CLI exit codes become Python exceptions
5. **JSON Parsing:** Parse CLI stdout as JSON and return

## Error Handling

**CLI Failures:** All CLI exit codes != 0 raise `CLIError` with exit code and stderr message.

**No Python-Specific Errors:** Python bindings do not add validation, error recovery, or fallback behavior.

## Determinism Requirements

**Identical Execution:** Same Python call must produce identical results to equivalent CLI invocation.

**Golden Equivalence:** Automated tests verify byte-for-byte JSON output equivalence between CLI and Python.

**No Environment Dependencies:** Python bindings resolve nx-cli executable deterministically without PATH or environment variables.

## Maintenance Contract

**CLI Changes Break Python:** Any modification to CLI command syntax, flags, or output format requires corresponding Python binding updates.

**No Independent Evolution:** Python bindings cannot add features, change behavior, or provide convenience methods beyond CLI mirroring.

**Version Coupling:** Python bindings are tightly coupled to specific CLI versions and must be updated synchronously.