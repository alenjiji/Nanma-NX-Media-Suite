# NX-Core — Foundational Module

## Purpose

NX-Core provides foundational primitives and infrastructure for all Nanma NX-MediaSuite components. It serves as the base layer that all engines and higher-level components depend upon.

## Architectural Position

NX-Core occupies Layer 1 in the strict four-layer architecture:
- **Layer 1: Core Engine** ← NX-Core resides here
- Layer 2: Business Logic
- Layer 3: API & Automation  
- Layer 4: User Interface

## Dependency Rules

- **All NX components depend on NX-Core**
- **NX-Core depends on NOTHING above it**
- NX-Core may only depend on standard C++20 library and platform APIs
- No Qt dependencies permitted
- No UI framework dependencies permitted

## Explicit Non-Goals

NX-Core deliberately does NOT contain:

- **Media Processing Logic**: No audio, video, or metadata processing
- **Engine Logic**: No transcoding, encoding, or conversion algorithms  
- **Component-Specific Code**: No Convert Pro, AudioLab, VideoTrans, MetaFix, BatchFlow, or Monitor logic
- **UI Dependencies**: No Qt, no GUI frameworks, no user interface code
- **Batch Processing**: No job scheduling, workflow, or automation logic
- **Monitoring**: No telemetry, logging, or observability features
- **Policy Logic**: No lossless policies, processing decisions, or quality assessments

## What IS Permitted in NX-Core

- Foundational data structures and primitives
- Platform abstraction utilities
- Memory management utilities
- Basic system interfaces
- Core namespace definitions
- Build system infrastructure

## Enforcement

Any code that requires knowledge of:
- Media formats or codecs
- Processing pipelines or workflows  
- Component-specific requirements
- User interface concerns

**MUST NOT** be placed in NX-Core.

## Build Requirements

- C++20 compatible compiler
- CMake 3.20 or higher
- No external dependencies beyond standard library
- Buildable as standalone static or shared library

## Usage

All NX components include NX-Core as their foundational dependency:

```cpp
#include <nx_core.h>
```

NX-Core provides the `nx::core` namespace for all foundational primitives.