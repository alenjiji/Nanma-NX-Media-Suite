# Phase 14 — CLI Batch Introspection Contract

## Objective

Define frozen CLI contract for batch introspection commands as read-only adapters over Phase 12 batch engine.

## Command Contract

### nx batch status
- **Purpose**: Query overall batch engine status
- **Phase 12 Source**: `BatchEngine::get_status()`
- **Output DTO**: `BatchStatusDto`
- **Serializers**: `BatchStatusJsonSerializer`, `BatchStatusTextSerializer`
- **Format Flags**: `--json`, `--text` (explicit, mutually exclusive)

### nx batch session <session_id>
- **Purpose**: Query specific batch session details
- **Phase 12 Source**: `BatchEngine::get_session(SessionId)`
- **Output DTO**: `BatchSessionDto`
- **Serializers**: `BatchSessionJsonSerializer`, `BatchSessionTextSerializer`
- **Format Flags**: `--json`, `--text` (explicit, mutually exclusive)
- **Parameter**: `<session_id>` - string identifier for batch session

### nx batch jobs <session_id>
- **Purpose**: Query jobs within specific batch session
- **Phase 12 Source**: `BatchEngine::get_session_jobs(SessionId)`
- **Output DTO**: `BatchJobsDto`
- **Serializers**: `BatchJobsJsonSerializer`, `BatchJobsTextSerializer`
- **Format Flags**: `--json`, `--text` (explicit, mutually exclusive)
- **Parameter**: `<session_id>` - string identifier for batch session

## Contract Constraints

### Read-Only Guarantee
- Commands never modify batch engine state
- Commands never trigger job execution
- Commands never create or delete sessions
- Commands only query existing state

### Deterministic Guarantee
- Same Phase 12 state → identical CLI output
- Same parameters → identical behavior
- No time-dependent output variations
- Stable field ordering in all outputs

### Parameter Validation
- `<session_id>` must be non-empty string
- Invalid session IDs → `ArtifactNotFound` error
- Missing session IDs → `InvalidCommand` error
- Extra parameters → `InvalidCommand` error

### Format Requirements
- Explicit format flag required (`--json` or `--text`)
- No format flag → `InvalidCommand` error
- Multiple format flags → `InvalidCommand` error
- Unknown flags → `InvalidCommand` error

## Error Mapping

### Command Errors
- Missing format flag → `CliError::InvalidCommand` (exit 64)
- Invalid session ID format → `CliError::InvalidCommand` (exit 64)
- Missing required parameters → `CliError::InvalidCommand` (exit 64)

### Phase 12 Errors
- Batch engine unavailable → `CliError::EngineUnavailable` (exit 10)
- Session not found → `CliError::ArtifactNotFound` (exit 11)
- Malformed session data → `CliError::ArtifactMalformed` (exit 12)

## Implementation Pipeline

### Data Flow
1. Parse command and validate parameters
2. Validate format flags (explicit, mutually exclusive)
3. Call Phase 12 batch engine adapter
4. Convert Phase 12 result to CLI DTO
5. Serialize using appropriate serializer
6. Write serialized output to stdout

### Adapter Interface
- `BatchQueryAdapter::query_status(const BatchEngine&)`
- `BatchQueryAdapter::query_session(const BatchEngine&, const std::string&)`
- `BatchQueryAdapter::query_session_jobs(const BatchEngine&, const std::string&)`

### DTO Structure
- DTOs mirror Phase 12 types exactly
- No CLI-specific fields or transformations
- Deterministic field ordering
- Direct copy construction from Phase 12 types

## Architectural Boundaries

### Phase 12 Dependency
- CLI depends only on Phase 12 public interface
- No knowledge of Phase 11 policies
- No knowledge of Phase 10 execution details
- Engine-agnostic implementation

### Serialization Contract
- JSON: compact object format, lexicographic key ordering
- Text: line-based `key=value` format, DTO field ordering
- No pretty-printing or formatting options
- Deterministic numeric and boolean serialization

## Testing Requirements

### Golden Tests
- One golden test per command per format
- Byte-exact comparison against golden files
- Fixed test data for deterministic outputs
- CLI binary invocation via process execution

### Contract Tests
- Parameter validation for all commands
- Format flag validation
- Error code stability
- Phase 12 adapter isolation