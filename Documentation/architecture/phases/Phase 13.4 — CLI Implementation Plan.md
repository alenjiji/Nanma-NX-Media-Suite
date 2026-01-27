# Phase 13 — CLI Implementation Plan

## Module Layout

```
nx-policy/
├── cli/
│   ├── main.cpp              # argument dispatch only
│   ├── CommandRegistry.h     # static command → output mapping
│   ├── CommandRegistry.cpp
│   ├── CommandNames.h        # frozen command strings
│   ├── OutputTypes.h         # forward declarations only
│   └── ExitCodes.h           # deterministic exit codes
│
├── adapters/
│   └── Phase12Reader.h       # read-only access facade
│
└── docs/
    └── phase13/
        ├── cli_boundary.md
        ├── cli_taxonomy.md
        ├── cli_mapping.md
        └── cli_output_types.md
```

## Responsibility Boundaries

### main.cpp
- Parse command line arguments
- Dispatch to CommandRegistry
- Return deterministic exit codes
- No business logic

### CommandRegistry
- Static mapping: command string → Phase 12 artifact type
- Command validation (exists/not exists)
- No command execution
- No output formatting

### CommandNames.h
- Frozen string constants for all 21 commands
- Immutable after Phase 13 lock
- Single source of truth for command names

### OutputTypes.h
- Forward declarations for 21 output types
- No implementation details
- Contract boundary for consumers

### ExitCodes.h
- Deterministic exit codes for all scenarios
- Stable across executions
- Machine-readable status

### Phase12Reader
- Read-only facade over Phase 12 engine
- Artifact retrieval only
- No interpretation or transformation
- Engine-agnostic interface

## Deterministic Exit Codes

```cpp
enum class ExitCode : int {
    Success = 0,
    
    // Command Issues
    UnknownCommand = 1,
    InvalidArguments = 2,
    
    // Phase 12 Issues
    EngineUnavailable = 10,
    ArtifactNotFound = 11,
    ArtifactMalformed = 12,
    
    // System Issues
    IOError = 20,
    MemoryError = 21,
    
    // Internal Issues
    InternalError = 99
};
```

## Testing Strategy

### Unit Tests
- CommandRegistry mapping validation
- CommandNames string constants verification
- ExitCode determinism across runs
- Phase12Reader facade isolation

### Integration Tests
- End-to-end command execution
- Phase 12 artifact retrieval
- Output type serialization
- Exit code consistency

### Contract Tests
- CLI command → Phase 12 artifact mapping
- Output type → Phase 12 artifact correspondence
- Deterministic behavior validation
- Engine-agnostic operation

### Regression Tests
- Command name stability
- Exit code stability
- Output format stability
- Performance characteristics

## Implementation Constraints

### Read-Only Guarantee
- No Phase 12 engine modification
- No file system writes
- No state mutation
- No side effects

### Deterministic Guarantee
- Same input → same output
- Same error → same exit code
- Stable command ordering
- Reproducible behavior

### Engine Agnostic Guarantee
- No engine-specific code
- Generic Phase 12 interface
- Pluggable engine support
- No engine assumptions

## Build Integration

### CMake Structure
```
nx-policy/
├── CMakeLists.txt           # main build configuration
├── cli/CMakeLists.txt       # CLI executable
└── tests/CMakeLists.txt     # test suite
```

### Dependencies
- Phase 12 engine (read-only interface)
- C++20 standard library only
- No external dependencies

### Artifacts
- `nx-policy` executable
- Unit test suite
- Integration test suite
- Documentation bundle