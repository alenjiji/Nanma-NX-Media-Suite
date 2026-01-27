# Nanma NX-MediaSuite — Architectural Principles

This is a modular, C++20-based, studio-grade media processing suite with strict lossless guarantees, explicit pipelines, no implicit transformations, and a shared core engine. Each component is a specialization over a common media graph + job scheduler architecture. UI and automation layers are adapters only.

## Introduction & Design Intent

This document establishes the immutable architectural principles governing Nanma NX-MediaSuite, a professional media processing system designed for studio-grade operations. These principles form a permanent engineering contract that must be upheld across all development phases.

The architecture prioritizes accuracy, determinism, and long-term maintainability over convenience or rapid development. Every design decision must serve the primary goal of delivering lossless, reproducible media processing capabilities that remain stable and predictable over a 10-20 year operational lifespan.

Lossless and deterministic design is critical because media professionals require absolute confidence in their tools. A single undetected quality loss or non-reproducible result can compromise entire productions, waste significant resources, and damage professional reputations. This system must be a trusted foundation that never introduces uncertainty into media workflows.

## Core Architectural Philosophy

### Accuracy Over Convenience
All system behaviors must prioritize correctness over ease of implementation or user convenience. When trade-offs arise, accuracy takes precedence without exception.

### Explicit Over Implicit Behavior
Every operation, transformation, and side effect must be explicitly declared and visible. The system must never perform hidden operations that could affect media quality or processing outcomes.

### Transparency and Predictability
All system behaviors must be observable, logged, and predictable. Users must be able to understand exactly what the system will do before execution and verify what it actually did after completion.

### No Hidden Transformations
The system must never apply automatic corrections, normalizations, or optimizations without explicit user authorization and full disclosure of the operations performed.

## Strict Layered Architecture

The system enforces a rigid four-layer architecture with unidirectional dependencies:

### Layer 1: Core Engine
- Media processing algorithms
- Format handlers and codecs
- Mathematical operations
- Hardware abstraction
- No dependencies on higher layers

### Layer 2: Business Logic
- Workflow orchestration
- Job management
- Validation and safety checks
- Depends only on Core Engine

### Layer 3: API & Automation
- Python bindings
- Batch processing interfaces
- Scripting capabilities
- Depends on Business Logic and Core Engine

### Layer 4: User Interface
- Qt-based presentation layer
- User interaction handling
- Visual feedback systems
- Depends on all lower layers through adapters only

### Dependency Rules
- Lower layers must never depend on higher layers
- Cross-layer communication occurs only through defined interfaces
- Direct access to non-adjacent layers is forbidden
- All layer boundaries must be testable in isolation

## Qt-Free Core Mandate

### Allowed in Core Engine
- Standard C++20 library components
- Platform-specific system APIs for hardware access
- Third-party libraries with no GUI dependencies
- Custom data structures and algorithms

### Explicitly Forbidden in Core Engine
- Any Qt classes or headers
- GUI-related dependencies of any kind
- Threading models tied to UI frameworks
- Event systems designed for user interfaces

### Rationale
Qt dependencies in the core engine would compromise testability, portability, and long-term maintainability. The core must remain independent of UI frameworks to enable headless operation, automated testing, and potential future UI technology migrations.

## Explicit Media Pipeline Design

### Media Processing as Explicit Graphs
All media operations must be represented as directed acyclic graphs where each node represents a single, well-defined transformation. Pipeline construction must be explicit and user-controlled.

### No Implicit Operations
The system must never automatically perform:
- Format conversions
- Sample rate changes
- Color space transformations
- Quality adjustments
- Metadata modifications

### Transformation Declaration
Every transformation must be:
- Explicitly requested by the user or automation script
- Logged with complete parameter details
- Reversible or documented as irreversible
- Validated before execution

## Lossless-First Policy Enforcement

### Definition of Lossless
In this system, "lossless" means no degradation of the original media data occurs during processing. This includes preservation of:
- Bit-exact audio and video data
- Complete metadata
- Original container structure when possible
- Timing information and synchronization

### Processing Hierarchy
1. **Passthrough**: Direct copying with no processing (preferred)
2. **Remux**: Container format changes without re-encoding
3. **Lossless Re-encode**: Mathematically reversible transformations only
4. **Lossy Re-encode**: Explicitly authorized quality reduction

### Re-encoding Authorization
Lossy operations require:
- Explicit user confirmation
- Clear documentation of quality impact
- Preservation of original files
- Detailed logging of all parameters used

## Deterministic Execution & Reproducibility

### Definition of Determinism
Given identical inputs and parameters, the system must produce bit-identical outputs across different executions, hardware configurations, and time periods.

### Reproducibility Requirements
- All operations must be repeatable with identical results
- Processing order must be consistent and documented
- Random number generation must use seeded, documented algorithms
- Threading must not affect output determinism

### Constraints
- No reliance on system time for processing decisions
- No hardware-specific optimizations that affect output
- No undefined behavior in algorithms
- All floating-point operations must use consistent precision

## Hardware Abstraction & Performance Safety

### Abstraction Principles
Hardware-specific optimizations must be isolated in the Core Engine layer behind abstract interfaces. Business logic must never contain hardware-specific code paths.

### Safe Fallback Behavior
Every hardware-accelerated operation must have a software fallback that produces identical results. Performance optimizations must never compromise correctness.

### Resource Management
- Memory allocation must be predictable and bounded
- CPU and GPU usage must be controllable and observable
- I/O operations must be asynchronous and cancellable
- Resource exhaustion must be handled gracefully

## Batch-First & Automation-Ready Design

### Batch Operation Requirements
Every interactive operation must have a corresponding batch-capable implementation. User interfaces are convenience layers over automation-ready core functionality.

### Preset and Job-Graph Philosophy
All processing workflows must be serializable as job graphs that can be:
- Saved and loaded
- Modified programmatically
- Executed without user intervention
- Validated before execution

### Headless Operation
The system must function completely without any user interface, supporting:
- Command-line execution
- Scripted automation
- Server deployment
- Continuous integration workflows

## Observability, Logging & Diagnostics

### Structured Logging Requirements
All system operations must generate structured, machine-readable logs containing:
- Operation timestamps
- Input and output file information
- Processing parameters
- Performance metrics
- Error conditions and recovery actions

### Deterministic Error Codes
Every error condition must have a unique, stable identifier that remains consistent across system versions. Error messages must be actionable and specific.

### Telemetry Integration
Core system metrics must be available for monitoring:
- Processing throughput
- Resource utilization
- Error rates and types
- Performance bottlenecks

## Extensibility & Long-Term Maintainability

### Format and Codec Extension
New media formats must be addable through plugin interfaces without modifying core engine code. Plugin APIs must remain stable across minor version updates.

### ABI/API Stability
Public interfaces must maintain backward compatibility within major version releases. Breaking changes require major version increments and migration documentation.

### Modular Design
System components must be independently testable, updatable, and replaceable. Dependencies between modules must be minimized and clearly documented.

## Non-Goals & Explicit Rejections

### Deliberately Avoided Features
- Real-time preview optimization at the expense of accuracy
- Automatic quality enhancement or "improvement" algorithms
- Platform-specific UI optimizations in core logic
- Convenience features that hide processing details

### Rejected Anti-Patterns
- Singleton pattern for core system components
- Global state for processing parameters
- Implicit type conversions in media data handling
- Exception-based control flow in performance-critical paths

### Explicitly Rejected Technologies
- Frameworks that couple business logic to UI concerns
- Libraries with unpredictable or non-deterministic behavior
- Tools that prioritize convenience over transparency
- Solutions that cannot be automated or scripted

This architectural foundation ensures Nanma NX-MediaSuite remains a reliable, professional-grade tool capable of meeting the demanding requirements of media production environments while maintaining the flexibility needed for long-term evolution and enhancement.