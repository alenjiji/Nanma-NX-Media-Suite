# NX-Convert Pro — Engineering Design Document

## 1. Component Overview

NX-Convert Pro is the flagship lossless transcoder within Nanma NX-MediaSuite, designed to perform accurate, policy-driven media conversion with zero unintended modification. It serves as the primary tool for format conversion, container remuxing, and codec transcoding operations in professional media workflows.

NX-Convert Pro exists to solve the critical problem of media format conversion without quality degradation or unexpected transformations. Professional media workflows require absolute confidence that conversion operations preserve the original media data while adapting it to different technical requirements.

The component is considered the "flagship" transcoder because it embodies the core architectural principles of the entire suite: lossless-first processing, deterministic execution, and explicit pipeline construction. It demonstrates how complex media operations can be performed with complete transparency and predictability.

## 2. Design Intent & Guarantees

NX-Convert Pro provides the following immutable guarantees:

- **Lossless-First Behavior**: All operations prioritize preservation of original media data. Lossy operations require explicit authorization and are performed only when no lossless alternative exists.

- **Deterministic Execution**: Given identical inputs and parameters, the system produces bit-identical outputs across different executions, hardware configurations, and time periods.

- **Explicit Pipeline Construction**: Every processing operation is represented as a visible, auditable pipeline graph. No hidden transformations or automatic corrections are performed.

- **Zero Hidden Media Modification**: The system never applies automatic gain control, color correction, sample rate conversion, or any other modification without explicit user authorization.

- **Complete Auditability**: Every operation is logged with full parameter details, enabling complete reconstruction of processing decisions and verification of results.

## 3. Core Engine Responsibility

The `nx::convert::TranscodeEngine` serves as the central orchestrator for all transcoding operations within NX-Convert Pro.

### Engine Ownership
- Pipeline graph construction and validation
- Stream analysis coordination
- Policy resolution and enforcement
- Execution scheduling and monitoring
- Output verification and reporting

### Delegated Responsibilities
- Media format detection (delegated to NX-Core format handlers)
- Codec implementation (delegated to NX-Core codec libraries)
- Hardware acceleration (delegated to NX-Core hardware abstraction)
- File I/O operations (delegated to NX-Core I/O subsystem)

### Forbidden Responsibilities
- Direct codec implementation
- Hardware-specific optimizations
- User interface concerns
- File system operations beyond coordination
- Quality assessment or enhancement decisions

## 4. Internal Pipeline Architecture

The internal pipeline follows a strict six-stage architecture:

### InputContainer
**Purpose**: Validates and prepares input media for analysis  
**Inputs**: File path, format hints, validation parameters  
**Outputs**: Validated container object with stream metadata  
**Constraints**: Must detect format without modification or assumption  
**Rationale**: Ensures input integrity before processing begins  
**Forbidden**: Format conversion, metadata modification, stream reordering

### StreamAnalyzer
**Purpose**: Analyzes individual streams to determine processing requirements  
**Inputs**: Container streams, target format specifications  
**Outputs**: Stream analysis reports with compatibility assessments  
**Constraints**: Analysis must be read-only and deterministic  
**Rationale**: Provides foundation for lossless policy decisions  
**Forbidden**: Stream modification, quality assessment, automatic corrections

### LosslessPolicyResolver
**Purpose**: Determines optimal processing path for each stream based on lossless-first principles  
**Inputs**: Stream analysis reports, user preferences, target specifications  
**Outputs**: Processing policy decisions for each stream  
**Constraints**: Must prioritize lossless operations and require explicit authorization for lossy operations  
**Rationale**: Enforces architectural principle of lossless-first processing  
**Forbidden**: Automatic quality trade-offs, hidden lossy operations, convenience shortcuts

### PipelineGraphBuilder
**Purpose**: Constructs explicit processing graph based on policy decisions  
**Inputs**: Policy decisions, stream specifications, processing constraints  
**Outputs**: Validated processing graph with defined node connections  
**Constraints**: Graph must be acyclic and fully deterministic  
**Rationale**: Provides transparent, auditable representation of all processing operations  
**Forbidden**: Implicit operations, hidden nodes, optimization shortcuts that affect output

### GraphExecutor
**Purpose**: Executes the processing graph while maintaining deterministic behavior  
**Inputs**: Validated processing graph, execution parameters  
**Outputs**: Processed streams, execution metrics, verification data  
**Constraints**: Execution must be repeatable and produce identical results  
**Rationale**: Ensures reliable, predictable processing execution  
**Forbidden**: Non-deterministic optimizations, execution shortcuts, result caching that affects determinism

### OutputContainer
**Purpose**: Assembles processed streams into final output format with verification  
**Inputs**: Processed streams, container specifications, verification requirements  
**Outputs**: Final media file, verification report, processing audit trail  
**Constraints**: Output must match specifications exactly with complete verification  
**Rationale**: Guarantees output integrity and provides audit trail  
**Forbidden**: Format assumptions, automatic corrections, metadata inference

## 5. Key Classes & Responsibilities

### StreamAnalyzer
**Primary Responsibility**: Analyzes media streams to determine their characteristics and processing requirements without modification.  
**Inputs**: Raw stream data, format specifications  
**Outputs**: Stream analysis reports containing codec, resolution, sample rate, and compatibility information  
**Allowed Behavior**: Read-only analysis, format detection, compatibility assessment  
**Forbidden Behavior**: Stream modification, quality assessment, automatic format conversion  
**Interactions**: Receives streams from InputContainer, provides analysis to LosslessPolicyResolver  
**Ownership**: Stateless analysis operations, no persistent data ownership

### LosslessPolicy
**Primary Responsibility**: Encapsulates decision logic for maintaining lossless processing while meeting output requirements.  
**Inputs**: Stream analysis data, target format specifications, user preferences  
**Outputs**: Processing decisions specifying passthrough, remux, or encode operations  
**Allowed Behavior**: Policy evaluation, compatibility checking, lossless path identification  
**Forbidden Behavior**: Quality trade-off decisions, automatic lossy fallbacks, user preference overrides  
**Interactions**: Consumes StreamAnalyzer output, provides decisions to PipelineGraphBuilder  
**Ownership**: Policy state during decision process, no persistent stream ownership

### TranscodeGraph
**Primary Responsibility**: Represents the complete processing pipeline as an explicit, auditable graph structure.  
**Inputs**: Policy decisions, stream routing requirements, processing constraints  
**Outputs**: Executable graph with defined processing nodes and connections  
**Allowed Behavior**: Graph construction, validation, dependency resolution  
**Forbidden Behavior**: Automatic optimization, hidden operations, execution shortcuts  
**Interactions**: Built by PipelineGraphBuilder, executed by GraphExecutor  
**Ownership**: Complete graph structure and node relationships during processing lifecycle

### StreamPassthroughNode
**Primary Responsibility**: Handles direct stream copying without any processing or modification.  
**Inputs**: Source stream data, target container requirements  
**Outputs**: Identical stream data in target container format  
**Allowed Behavior**: Bit-exact copying, container format adaptation  
**Forbidden Behavior**: Any data modification, optimization, or enhancement  
**Interactions**: Receives data from source, provides data to OutputContainer  
**Ownership**: Temporary stream data during copy operation only

### DecodeNode
**Primary Responsibility**: Decodes compressed media streams to uncompressed format for further processing.  
**Inputs**: Compressed stream data, decoding parameters  
**Outputs**: Uncompressed media data with complete fidelity  
**Allowed Behavior**: Lossless decoding, format validation, error detection  
**Forbidden Behavior**: Quality enhancement, automatic correction, lossy shortcuts  
**Interactions**: Receives compressed data, provides uncompressed data to processing nodes  
**Ownership**: Temporary decoded data during processing pipeline execution

### EncodeNode
**Primary Responsibility**: Encodes uncompressed media data using specified codec with explicit quality parameters.  
**Inputs**: Uncompressed media data, encoding specifications, quality parameters  
**Outputs**: Encoded stream data with documented quality characteristics  
**Allowed Behavior**: Encoding with specified parameters, quality verification, deterministic output  
**Forbidden Behavior**: Automatic quality adjustment, undocumented optimizations, non-deterministic encoding  
**Interactions**: Receives uncompressed data from DecodeNode or processing nodes, provides encoded data to OutputContainer  
**Ownership**: Temporary encoded data and encoding state during operation

### RemuxNode
**Primary Responsibility**: Changes container format without re-encoding stream data.  
**Inputs**: Stream data in source container, target container specifications  
**Outputs**: Identical stream data in target container format  
**Allowed Behavior**: Container format conversion, metadata preservation, timing adjustment  
**Forbidden Behavior**: Stream re-encoding, quality modification, metadata inference  
**Interactions**: Receives containerized streams, provides recontainerized streams to OutputContainer  
**Ownership**: Temporary container structures during remux operation

## 6. Lossless Policy System

The lossless policy system implements a hierarchical decision process that prioritizes data preservation while meeting output requirements. The system evaluates each stream independently and selects the processing path that maintains the highest fidelity possible.

The policy evaluation follows this logic: when the input codec matches the output codec and the target container supports that codec, the system performs a remux operation that preserves the original encoded data. If the codecs differ but a lossless codec is available for the target format, the system decodes the source and re-encodes using the lossless codec. When only lossy codecs are available for the required output format, the system requires explicit user authorization before proceeding.

**Policy Logic**:
If input codec equals output codec and the container supports it → remux  
Else if a lossless codec is available → lossless encode  
Else → explicit user override is required

This approach ensures that quality degradation occurs only when absolutely necessary and with full user awareness. The system maintains detailed logs of all policy decisions, enabling users to understand exactly why specific processing paths were chosen and to verify that their quality requirements are being met.

## 7. Stream Independence Model

NX-Convert Pro processes audio and video streams independently to maximize correctness, flexibility, and determinism. Each stream is analyzed, processed, and validated separately, allowing different processing policies to be applied based on the specific characteristics and requirements of each stream type.

Independent stream processing improves correctness by preventing cross-stream dependencies that could introduce unexpected behavior or quality degradation. It enhances flexibility by allowing optimal processing paths for each stream type without compromise. Determinism is improved because stream processing order does not affect individual stream outcomes.

Multi-stream scenarios are fully supported, including multiple audio tracks, subtitle streams, and metadata tracks. Each stream receives independent analysis and policy evaluation, ensuring that complex media files with diverse stream types can be processed optimally while maintaining the lossless-first principle for each component stream.

## 8. Determinism & Validation

Deterministic behavior is enforced through strict control of all processing variables that could affect output. The system uses seeded random number generators for any operations requiring randomness, maintains consistent processing order regardless of system threading, and eliminates dependencies on system time or hardware-specific optimizations that could vary between executions.

Output determinism requires that bit-exact identical results are produced across different executions given identical inputs and parameters. This includes identical file sizes, checksums, stream timing, and metadata organization. The system validates determinism by computing and comparing cryptographic hashes of all output streams and metadata.

Validation is performed at multiple levels: input validation ensures source integrity before processing begins, intermediate validation verifies that each processing stage produces expected results, and output validation confirms that final results match specifications and quality requirements.

Determinism is critical for professional workflows because it enables reliable batch processing, automated quality assurance, and reproducible results across different systems and time periods. Media professionals must be able to trust that identical processing operations will produce identical results regardless of when or where they are executed.

## 9. Outputs & Verification Artifacts

NX-Convert Pro produces comprehensive outputs and verification artifacts for every conversion operation:

**Conversion Report**: Complete documentation of all processing decisions, including policy choices, processing paths selected, and quality parameters used. This report enables full reconstruction of the conversion process and serves as an audit trail for quality assurance.

**Stream-Level Hashes**: Cryptographic checksums computed for each individual stream before and after processing. These hashes enable verification that streams were processed correctly and detection of any unexpected modifications.

**Bit-Depth Verification**: Documentation of bit depth preservation or conversion for audio streams, including verification that no precision loss occurred during processing. This artifact is critical for maintaining audio fidelity in professional workflows.

**Sample-Rate Verification**: Confirmation that audio sample rates were preserved or converted as specified, with documentation of any resampling operations performed. This ensures that timing and frequency characteristics are maintained correctly.

**Error and Warning Reporting**: Detailed logs of any issues encountered during processing, including recoverable warnings and critical errors. This reporting enables troubleshooting and quality assurance validation.

Each artifact serves specific verification and audit requirements in professional media workflows, enabling users to validate that processing operations met their quality and technical requirements.

## 10. Explicit Non-Goals

NX-Convert Pro deliberately excludes the following capabilities to maintain focus on accurate, transparent transcoding:

- **Audio Mastering**: No automatic gain adjustment, dynamic range compression, or loudness normalization. These operations require creative decisions that are outside the scope of format conversion.

- **Loudness Normalization**: No automatic adjustment of audio levels to meet broadcast standards or streaming platform requirements. Such operations must be performed by dedicated mastering tools with appropriate creative oversight.

- **Color Grading**: No automatic color correction, white balance adjustment, or color space conversion beyond what is required for format compatibility. Color decisions require creative input and specialized tools.

- **Metadata Authoring**: No automatic generation or enhancement of metadata beyond safe passthrough of existing metadata. Metadata creation and editing require specific knowledge of content and intended use.

- **Quality Enhancement Features**: No automatic sharpening, noise reduction, upscaling, or other "improvement" algorithms. Such operations introduce subjective changes that compromise the lossless-first principle.

- **Format Optimization**: No automatic selection of "better" codecs or quality settings based on content analysis. All processing decisions must be explicit and user-controlled.

- **Batch Intelligence**: No automatic adaptation of processing parameters based on content characteristics or previous processing results. Each operation must be explicitly specified and deterministic.