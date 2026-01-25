# NX-MetaFix Engine - Phase 1.A Skeleton

**NO LOGIC — PHASE 1.A**  
**Compile-only skeleton**  
**Media essence is read-only by default**  
**Deterministic by construction**

This is the core engine skeleton for NX-MetaFix, the metadata and container repair engine within Nanma NX-MediaSuite.

## Current Status: Compile-Only Skeleton

This phase introduces:
- API contract definition only
- Deterministic APIs using nx-core systems
- Result-based flow for metadata operations
- LogicalClock integration
- **Zero repair or processing logic**

This phase explicitly excludes:
- Processing or repair logic implementation
- Media essence modification in any form
- Codecs, remuxing, or format conversion
- IO, filesystem, or network operations
- Inference, heuristics, or automatic behavior
- Execution or scheduling logic

## Architecture

### Core Types
- `MetaRepairRequest` - Intent to perform structural/metadata operations (NOT execution)
- `MetaRepairResult` - Deterministic outcome of repair planning (NOT repaired files)
- `MetaEngine` - Pure coordinator class (no logic)

### Metadata-Specific ID Value Types (Strong Types)
- `ContainerId` - Container identification
- `MetadataNodeId` - Metadata node identification  
- `RepairGraphId` - Repair graph identification

### Design Principles
- **Media Essence Protection**: Read-only by default
- **Deterministic**: Same input = same output, always
- **Value Types Only**: No hidden state or side effects
- **LogicalClock Only**: No wall-clock time dependencies
- **Result-Based Flow**: No exceptions for control flow

## Semantic Clarity

**MetaRepairRequest represents:**
- Intent to perform structural or metadata operations
- NOT execution
- NOT modification of media essence
- NOT automatic correction

**MetaRepairResult represents:**
- Deterministic outcome of repair planning or validation
- NOT repaired files
- NOT runtime execution results

## Compliance Verification

| Rule              | Status |
| ----------------- | ------ |
| NX-Core untouched | ✅      |
| No dependencies on other engines | ✅      |
| Value types only  | ✅      |
| LogicalClock only | ✅      |
| Result-based flow | ✅      |
| No repair logic   | ✅      |
| Media essence read-only | ✅      |
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
- Container repair planning
- Metadata validation operations
- Structural repair coordination
- Compliance verification
- Audit trail generation

**NO LOGIC WILL BE ADDED UNTIL EXPLICITLY AUTHORIZED FOR NEXT PHASE**