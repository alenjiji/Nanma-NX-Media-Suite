# Phase 14A — CLI Batch Introspection Contract

**Status:** Phase 14A.1 Complete  
**Purpose:** Formal CLI contract for read-only batch introspection  
**Scope:** Lossless projection of existing batch artifacts via CLI  

---

## Command Namespace

All batch introspection commands use the dedicated namespace:

```
nx batch inspect <subcommand> [options]
```

**Rationale:** The `inspect` verb clearly indicates read-only operations and prevents confusion with execution verbs.

---

## Command Enumeration

### 1. Batch Plan Introspection

#### `nx batch inspect plan <batch_id>`
**Purpose:** Display batch plan structure and DAG  
**Output:** Complete batch plan with job dependencies  

**Flags:**
- `--format json|human` (default: json)
- `--jobs-only` - Show only job list without dependencies
- `--dag-only` - Show only dependency graph

**Output Schema:**
```json
{
  "batch_id": "string",
  "plan_hash": "string", 
  "job_count": "number",
  "jobs": [
    {
      "job_id": "string",
      "job_type": "string",
      "dependencies": ["string"],
      "execution_order": "number"
    }
  ],
  "dag": {
    "nodes": ["string"],
    "edges": [["string", "string"]]
  }
}
```

#### `nx batch inspect jobs <batch_id>`
**Purpose:** List all jobs in batch with basic metadata  
**Output:** Job inventory with identity and type information  

**Flags:**
- `--format json|human` (default: json)
- `--filter-type <job_type>` - Filter by job type
- `--sort execution|dependency|id` (default: execution)

**Output Schema:**
```json
{
  "batch_id": "string",
  "jobs": [
    {
      "job_id": "string",
      "job_type": "string", 
      "execution_order": "number",
      "dependency_count": "number",
      "dependent_count": "number"
    }
  ]
}
```

### 2. Execution State Introspection

#### `nx batch inspect status <batch_id>`
**Purpose:** Show materialized execution state for all jobs  
**Output:** Final execution states from completed batch  

**Flags:**
- `--format json|human` (default: json)
- `--filter-state success|failed|skipped` - Filter by final state
- `--include-retries` - Include retry count information

**Output Schema:**
```json
{
  "batch_id": "string",
  "execution_complete": "boolean",
  "job_states": [
    {
      "job_id": "string",
      "final_state": "success|failed|skipped",
      "retry_count": "number",
      "failure_classification": "string|null",
      "execution_duration_ms": "number|null"
    }
  ]
}
```

#### `nx batch inspect job <batch_id> <job_id>`
**Purpose:** Detailed view of single job execution  
**Output:** Complete job execution information  

**Flags:**
- `--format json|human` (default: json)
- `--include-artifacts` - Include artifact references
- `--include-timeline` - Include execution timeline

**Output Schema:**
```json
{
  "batch_id": "string",
  "job_id": "string",
  "job_type": "string",
  "final_state": "success|failed|skipped",
  "retry_count": "number",
  "failure_classification": "string|null",
  "execution_timeline": [
    {
      "timestamp": "string",
      "event": "string",
      "details": "object"
    }
  ],
  "artifacts": [
    {
      "artifact_id": "string",
      "artifact_type": "string",
      "size_bytes": "number"
    }
  ]
}
```

### 3. Policy Resolution Introspection

#### `nx batch inspect policies <batch_id>`
**Purpose:** Show resolved policy decisions for batch  
**Output:** Policy resolution artifacts (read-only view)  

**Flags:**
- `--format json|human` (default: json)
- `--job-id <job_id>` - Filter to specific job
- `--policy-type retry|failure|execution` - Filter by policy type

**Output Schema:**
```json
{
  "batch_id": "string",
  "policy_resolutions": [
    {
      "job_id": "string",
      "policy_type": "retry|failure|execution",
      "policy_applied": "string",
      "resolved_decision": "object",
      "resolution_timestamp": "string"
    }
  ]
}
```

### 4. Artifact Introspection

#### `nx batch inspect artifacts <batch_id>`
**Purpose:** List all artifacts produced by batch execution  
**Output:** Artifact inventory with metadata  

**Flags:**
- `--format json|human` (default: json)
- `--artifact-type report|validation|hash|timeline|log` - Filter by type
- `--job-id <job_id>` - Filter to specific job

**Output Schema:**
```json
{
  "batch_id": "string",
  "artifacts": [
    {
      "artifact_id": "string",
      "artifact_type": "report|validation|hash|timeline|log",
      "job_id": "string",
      "size_bytes": "number",
      "created_timestamp": "string",
      "content_hash": "string"
    }
  ]
}
```

#### `nx batch inspect artifact <batch_id> <artifact_id>`
**Purpose:** Display specific artifact content  
**Output:** Raw artifact content  

**Flags:**
- `--format json|raw` (default: raw)
- `--max-size <bytes>` - Limit output size (default: unlimited)

---

## Addressing Model

### Target Identification
- **Batch ID:** Required for all commands, must be exact match
- **Job ID:** Optional secondary selector, must be exact match  
- **Artifact ID:** Optional tertiary selector, must be exact match

### No Implicit Behavior
- No wildcard matching
- No partial ID resolution
- No live discovery or enumeration
- No default batch selection

### Error Handling
- Unknown batch ID: `ERROR_BATCH_NOT_FOUND`
- Unknown job ID: `ERROR_JOB_NOT_FOUND`  
- Unknown artifact ID: `ERROR_ARTIFACT_NOT_FOUND`
- Incomplete execution: `ERROR_EXECUTION_INCOMPLETE` (for state queries)

---

## Output Guarantees

### Deterministic Output
- Identical JSON structure for identical inputs
- Stable field ordering in JSON output
- Consistent timestamp formatting (ISO 8601)
- Deterministic array ordering (by ID, then by execution order)

### Machine-Readable First
- JSON is primary output format
- Human-readable format available via `--format human`
- All structured data preserved in JSON mode
- No information loss between formats

### Completeness
- No truncation without explicit `--max-size` flag
- All available fields included unless filtered
- Complete error context in failure cases
- Full artifact metadata always included

---

## Invariants & Non-Goals

### Invariants (Must Always Hold)
1. **Read-Only:** No command modifies any batch state
2. **Snapshot Consistency:** All data reflects materialized artifacts only
3. **Deterministic:** Same input always produces same output
4. **Complete:** All available information is accessible via CLI
5. **Addressable:** Every artifact has a unique, stable address

### Non-Goals (Explicitly Forbidden)
1. **Live State:** No runtime hooks into active schedulers or workers
2. **Mutation:** No commands that modify batch plans, jobs, or artifacts  
3. **Execution Control:** No start, stop, retry, resume, or cancel operations
4. **Inference:** No recomputation or smart interpretation of data
5. **Discovery:** No enumeration of available batches or live scanning
6. **Adaptive Behavior:** No context-sensitive or user-preference-driven output

### Boundary Conditions
- Commands fail fast on invalid input
- Missing artifacts return structured errors, not empty results
- Incomplete executions are explicitly identified, not inferred as complete
- Policy resolution shows final decisions only, never re-evaluates

---

## Implementation Requirements

### Data Sources (Consume Only)
- Batch plan artifacts from Phase 7-10
- Execution state records from Phase 10
- Policy resolution artifacts from Phase 11-12
- Telemetry and timeline data from monitoring
- Materialized reports and validation summaries

### CLI Integration
- Follows existing `nx` command structure from Phase 13
- Uses same error handling and output formatting patterns
- Integrates with existing JSON serialization framework
- Maintains same deterministic output guarantees

### Testing Requirements
- Golden tests for all output formats
- Error condition coverage for all failure modes
- Deterministic output validation across multiple runs
- Schema validation for all JSON outputs

---

**Contract Status:** ✅ Complete  
**Next Phase:** 14A.2 - Implementation of CLI batch introspection commands