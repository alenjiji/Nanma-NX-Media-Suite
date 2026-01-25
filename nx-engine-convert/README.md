# NX-Convert Pro Engine - Phase 1.A Skeleton

**PHASE 1.A — ENGINE SKELETON (NO LOGIC)**

This is the core engine skeleton for NX-Convert Pro, the flagship lossless transcoder within Nanma NX-MediaSuite.

## Current Status: Compile-Only Skeleton

This phase introduces:
- Type system and engine boundaries
- Deterministic APIs using nx-core systems
- Result/MultiResult flow
- LogicalClock integration
- **Zero execution logic**

This phase explicitly excludes:
- Any actual transcoding logic
- Stream analysis implementation
- Policy resolution logic
- Graph execution
- IO, codecs, containers
- System time dependencies
- Threading

## Architecture

### Core Types
- `TranscodeRequest` - Value type with LogicalClock and opaque handles
- `TranscodeResult` - Result wrapper around TranscodeOutcome
- `TranscodeGraph` - Data structure representing processing pipeline
- `TranscodeEngine` - Pure coordinator class (no logic)

### Design Principles
- **Deterministic**: Same input = same output, always
- **Value Types Only**: No hidden state or side effects
- **LogicalClock Only**: No wall-clock time dependencies
- **Result-Based Flow**: No exceptions for control flow
- **Lossless-First**: Architecture supports lossless processing priority

## Compliance Verification

| Rule              | Status |
| ----------------- | ------ |
| NX-Core untouched | ✅      |
| Qt-free           | ✅      |
| Value types only  | ✅      |
| LogicalClock only | ✅      |
| Result-based flow | ✅      |
| No IO             | ✅      |
| No policies       | ✅      |
| Compile-only      | ✅      |
| Deterministic     | ✅      |

## Build

```bash
cmake -B build -S . -DBUILD_TESTS=ON
cmake --build build
cd build && ctest
```

## Future Phases

This skeleton provides the foundation for:
- Stream analysis implementation
- Lossless policy resolution
- Pipeline graph execution
- Verification and audit trails

**NO LOGIC WILL BE ADDED UNTIL EXPLICITLY AUTHORIZED FOR NEXT PHASE**