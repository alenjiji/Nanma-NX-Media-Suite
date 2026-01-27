# Nanma NX-MediaSuite

Professional, studio-grade media processing suite designed for lossless, deterministic, and transparent media operations.

## Overview

Nanma NX-MediaSuite is a C++20-based media processing system built for professional broadcast, post-production, and archival workflows. The suite prioritizes accuracy over convenience, providing explicit control over every media transformation with complete auditability and deterministic execution.

## Core Principles

- **Lossless-First Processing**: All operations prioritize preservation of original media data
- **Deterministic Execution**: Identical inputs produce bit-identical outputs across different systems
- **Explicit Pipeline Construction**: Every transformation is visible and auditable
- **Zero Hidden Operations**: No automatic corrections or implicit processing
- **Professional Reliability**: Designed for 10-20 year operational lifespan

## Architecture

The suite follows a strict four-layer architecture:

1. **Core Engine** - Qt-free C++20 media processing algorithms
2. **Business Logic** - Workflow orchestration and validation
3. **API & Automation** - Python bindings and batch processing
4. **User Interface** - Qt-based presentation layer (adapter only)

## Components

### NX-Convert Pro
Flagship lossless transcoder for accurate, policy-driven media conversion.
- Format conversion without quality degradation
- Container remuxing and codec transcoding
- Explicit pipeline construction with complete auditability

### NX-AudioLab
High-resolution audio engine for sample-accurate professional audio processing.
- ITU-R BS.1770 compliant loudness measurement and normalization
- Deterministic sample rate conversion with polyphase filtering
- Broadcast-safe audio processing with clipping prevention

### NX-VideoTrans
Professional video encoding suite for color-safe, deterministic video processing.
- GPU-accelerated encoding without correctness loss
- Explicit color space management with no silent conversions
- Support for ProRes, DNxHR, and broadcast-grade encoding

### NX-MetaFix
Metadata and container repair engine for structural restoration.
- Container repair without touching media essence
- Metadata validation and normalization
- Broadcast compliance verification

### NX-BatchFlow
Intelligent batch processing and workflow automation engine.
- Deterministic job execution with explicit workflow definition
- DAG-based job scheduling with failure handling
- Folder watch automation with atomic file detection

### NX-Monitor
Real-time telemetry and observability engine.
- Zero-impact monitoring of all processing operations
- Comprehensive performance metrics and timeline analysis
- Deterministic error classification and diagnostics

## Key Features

### Deterministic Processing
- Bit-identical outputs across different executions
- Seeded algorithms for reproducible results
- Hardware-independent processing guarantees

### Professional Quality Assurance
- Comprehensive verification artifacts for every operation
- Complete audit trails with processing parameter logging
- Validation reports for compliance and quality control

### Batch-First Design
- All interactive operations have batch-capable implementations
- Headless operation support for server deployment
- Preset system with version control and backward compatibility

### Hardware Abstraction
- CPU and GPU acceleration with software fallbacks
- Platform-independent processing with consistent results
- Safe resource management with predictable performance

## Use Cases

- **Broadcast Pipeline Automation** - Daily content processing with compliance validation
- **Post-Production Workflows** - Format conversion and quality control for delivery
- **Archival Processing** - Long-term preservation with format migration
- **Studio Operations** - Repetitive processing tasks with consistent quality
- **Compliance Verification** - Broadcast and delivery requirement validation

## Technical Requirements

- **Operating System**: Windows, Linux, macOS
- **Compiler**: C++20 compatible compiler
- **Dependencies**: Qt (UI layer only), platform-specific media libraries
- **Hardware**: CPU with AVX2 support, optional GPU acceleration

## Development Status

Nanma NX-MediaSuite follows a strict phased development approach with immutable architectural contracts. See [Documentation/Roadmap.md](Documentation/Roadmap.md) for the complete implementation roadmap.

### Current Status: Phase 11 Complete - Execution Policies & Phase 12 In Progress

- **Phase Group A (Foundation)**: ✅ Complete & Locked - CLI, parsing, error handling
- **Phase Group B (Observability)**: ✅ Complete & Locked - Monitor engine for system visibility
- **Phase Group C (Orchestration)**: ✅ Complete & Locked - BatchEngine for deterministic batch planning
- **Phase Group E (Execution)**: ✅ Complete & Locked - Synchronous job execution and lifecycle management
- **Phase 10 (Deterministic Retries)**: ✅ Complete & Locked - Retry engine, execution persistence, and replay verification
- **Phase 11 (Execution Policies)**: ✅ Complete & Locked - Pure data model for execution constraints
- **Phase 12 (Policy Interpretation)**: ⏳ Current - Read-only policy interpretation engine
- **Phase 13-16 (Adapters & UI)**: 🔄 Future - CLI adapters, Python bindings, Qt UI, and project closure

### Architecture Maturity

The system has achieved **Phase 11 Complete** with:
- Complete CLI interface with deterministic parsing
- Real-time system monitoring and engine discovery
- Deterministic batch job planning with stable job IDs
- Full job execution engine with state management
- Complete job lifecycle with deterministic state transitions
- Deterministic retry engine with lineage tracking
- Execution persistence for forensic replay
- Replay verification harness for determinism validation
- Complete execution policy data model (Phase 11.1-11.3)

**Phase 11 Milestone Achieved**: Immutable policy framework with interpretation engine in progress.

## Documentation

- [Development Roadmap](Documentation/planning/Roadmap V1.md) - Final phased development plan and architectural contracts
- [Architectural Principles](Documentation/architecture/principles/architectural_principles.md) - Core design philosophy and constraints
- [NX-Convert Pro Design](Documentation/specifications/components/nx_convert_pro_design.md) - Lossless transcoding engine
- [NX-AudioLab Design](Documentation/specifications/components/nx_audiolab_design.md) - Professional audio processing
- [NX-VideoTrans Design](Documentation/specifications/components/nx_videotrans_design.md) - Video encoding suite
- [NX-MetaFix Design](Documentation/specifications/components/nx_metafix_design.md) - Metadata and container repair
- [NX-BatchFlow Design](Documentation/specifications/components/nx_batchflow_design.md) - Batch processing automation
- [NX-Monitor Design](Documentation/specifications/components/nx_monitor_design.md) - Telemetry and observability

## Professional Guarantees

### Media Essence Protection
- Original media data remains untouched unless explicitly authorized
- All modifications are logged with complete parameter disclosure
- Reversible operations where technically possible

### Processing Transparency
- Every operation is explicitly defined and auditable
- No hidden transformations or automatic corrections
- Complete processing reports with verification artifacts

### Reliability Assurance
- Deterministic behavior across different hardware configurations
- Comprehensive error handling with stable error codes
- Long-term compatibility with version migration support

## License

Professional media processing suite for studio and broadcast environments.

## Support

For technical support and professional services, contact the development team through official channels.