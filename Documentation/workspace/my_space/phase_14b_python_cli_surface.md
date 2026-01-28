# Phase 14B — Python-Exposed CLI Surface (v1)

**Status:** FROZEN for Python binding implementation  
**Source:** Actual CLI implementation analysis  
**Scope:** Read-only introspection commands only  

---

## Monitor Commands

### 1. `nx monitor status`
- **CLI Invocation:** `nx monitor status --json` OR `nx monitor status --text`
- **Positional Arguments:** None
- **Required Flags:** `--json` OR `--text` (mutually exclusive, one required)
- **Optional Flags:** None
- **Output Format:** JSON structured output (MonitorStatusDto) or human-readable text
- **Exit Code:** 0 on success, non-zero on CLI error

---

## Batch Introspection Commands (Phase 14A)

### 2. `nx batch inspect plan`
- **CLI Invocation:** `nx batch inspect plan <batch_id>`
- **Positional Arguments:** `batch_id` (required string)
- **Required Flags:** None
- **Optional Flags:** 
  - `--format json|human` (default: json)
  - `--jobs-only` (exclude DAG section)
  - `--dag-only` (exclude jobs section)
- **Output Format:** JSON with batch plan structure and dependency DAG
- **Exit Code:** 0 on success, ERROR_BATCH_NOT_FOUND if batch not found

### 3. `nx batch inspect jobs`
- **CLI Invocation:** `nx batch inspect jobs <batch_id>`
- **Positional Arguments:** `batch_id` (required string)
- **Required Flags:** None
- **Optional Flags:**
  - `--format json|human` (default: json)
  - `--filter-type <job_type>` (filter by job type string)
  - `--sort execution|dependency|id` (default: execution)
- **Output Format:** JSON with job inventory and metadata
- **Exit Code:** 0 on success, ERROR_BATCH_NOT_FOUND if batch not found

### 4. `nx batch inspect status`
- **CLI Invocation:** `nx batch inspect status <batch_id>`
- **Positional Arguments:** `batch_id` (required string)
- **Required Flags:** None
- **Optional Flags:**
  - `--format json|human` (default: json)
  - `--filter-state success|failed|skipped` (filter by final state)
  - `--include-retries` (include retry count information)
- **Output Format:** JSON with execution states for all jobs
- **Exit Code:** 0 on success, ERROR_BATCH_NOT_FOUND if batch not found, ERROR_EXECUTION_INCOMPLETE if execution not complete

### 5. `nx batch inspect job`
- **CLI Invocation:** `nx batch inspect job <batch_id> <job_id>`
- **Positional Arguments:** `batch_id` (required string), `job_id` (required string)
- **Required Flags:** None
- **Optional Flags:**
  - `--format json|human` (default: json)
  - `--include-artifacts` (include artifact references)
  - `--include-timeline` (include execution timeline)
- **Output Format:** JSON with detailed job execution information
- **Exit Code:** 0 on success, ERROR_BATCH_NOT_FOUND if batch not found, ERROR_JOB_NOT_FOUND if job not found, ERROR_EXECUTION_INCOMPLETE if execution not complete

### 6. `nx batch inspect policies`
- **CLI Invocation:** `nx batch inspect policies <batch_id>`
- **Positional Arguments:** `batch_id` (required string)
- **Required Flags:** None
- **Optional Flags:**
  - `--format json|human` (default: json)
  - `--job-id <job_id>` (filter to specific job)
  - `--policy-type retry|failure|execution` (filter by policy type)
- **Output Format:** JSON with policy resolution artifacts
- **Exit Code:** 0 on success, ERROR_BATCH_NOT_FOUND if batch not found

### 7. `nx batch inspect artifacts`
- **CLI Invocation:** `nx batch inspect artifacts <batch_id>`
- **Positional Arguments:** `batch_id` (required string)
- **Required Flags:** None
- **Optional Flags:**
  - `--format json|human` (default: json)
  - `--artifact-type report|validation|hash|timeline|log` (filter by type)
  - `--job-id <job_id>` (filter to specific job)
- **Output Format:** JSON with artifact inventory and metadata
- **Exit Code:** 0 on success, ERROR_BATCH_NOT_FOUND if batch not found

### 8. `nx batch inspect artifact`
- **CLI Invocation:** `nx batch inspect artifact <batch_id> <artifact_id>`
- **Positional Arguments:** `batch_id` (required string), `artifact_id` (required string)
- **Required Flags:** None
- **Optional Flags:**
  - `--format json|raw` (default: raw)
  - `--max-size <bytes>` (limit output size, default: unlimited)
- **Output Format:** Raw artifact content or JSON-wrapped content
- **Exit Code:** 0 on success, ERROR_BATCH_NOT_FOUND if batch not found, ERROR_ARTIFACT_NOT_FOUND if artifact not found

---

## Summary

**Total Commands:** 8  
**Command Groups:** 2 (monitor, batch inspect)  
**All Commands:** Read-only introspection only  
**Mutation Commands:** None (excluded by design)  
**Interactive Commands:** None (excluded by design)  

**Python Binding Requirements:**
- 1:1 CLI command mapping
- Identical argument validation
- Identical output format (JSON preferred)
- Identical error codes and exit behavior
- No Python-specific enhancements or shortcuts

**Source Verification:** Based on actual CLI implementation in CommandRegistry.cpp, CliApp.cpp, and Phase 14A contract specification.

**Status:** FROZEN - No additions, modifications, or semantic changes permitted.