# NX-VideoTrans — Engineering Design Document

## 1. Component Overview

NX-VideoTrans is the professional video encoding suite within Nanma NX-MediaSuite, designed for high-bitrate, color-safe, deterministic video encoding across professional, broadcast, mezzanine, and archival workflows. It provides GPU-accelerated video processing with strict guarantees for color correctness and deterministic output.

NX-VideoTrans exists to address the specialized requirements of professional video workflows that demand mathematical precision in color handling, deterministic frame processing, and complete transparency in all video transformations. Professional video production requires tools that maintain color accuracy across different display technologies, preserve HDR metadata without modification, and produce identical results across different encoding sessions.

The component is designed to solve video encoding problems that require sample-level accuracy in color processing, deterministic GOP structure generation, explicit control over scaling and color space operations, and hardware-accelerated encoding without sacrificing correctness. These operations require specialized algorithms and cannot be safely performed by consumer-oriented encoding tools that prioritize convenience over accuracy.

Professional video encoding requires stricter guarantees than consumer tools because video content represents significant financial investment, must maintain quality through multiple generations of processing, and must comply with strict broadcast and archival standards. A single undetected color shift or non-deterministic encoding artifact can compromise entire productions and violate broadcast compliance requirements.

## 2. Design Intent & Professional Guarantees

NX-VideoTrans provides the following immutable guarantees for professional video encoding:

- **Color Correctness and Safety**: All color processing operations maintain mathematical precision with no silent color space conversions, range changes, or HDR metadata modifications. Color transformations are performed only when explicitly authorized with full parameter disclosure.

- **Deterministic Frame Output**: Given identical inputs and parameters, the system produces bit-identical video outputs across different executions, hardware configurations, and GPU acceleration states. Frame ordering and GOP structure remain consistent regardless of processing environment.

- **Explicit Pipeline Construction**: Every video transformation is represented as a visible, auditable processing node. No hidden operations, automatic optimizations, or implicit processing steps are performed without explicit user authorization.

- **No Hidden Color, Scaling, or Quality Changes**: The system never applies automatic color correction, resolution changes, or quality adjustments without explicit user authorization and complete parameter documentation.

- **Broadcast- and Archive-Safe Behavior**: All processing operations maintain compliance with professional broadcast standards and archival requirements, including proper handling of color metadata, timing information, and GOP structure constraints.

## 3. Core Engine Responsibility

The `nx::video::VideoEngine` serves as the central coordinator for all video processing operations within NX-VideoTrans, maintaining strict adherence to professional video processing requirements.

### Engine Ownership
- Video processing pipeline construction and validation
- Color space transformation coordination and safety verification
- Frame scheduling and deterministic ordering enforcement
- Encoder abstraction and hardware acceleration management
- Video quality verification and compliance reporting

### Delegated Responsibilities
- Video format decoding and container handling (delegated to NX-Core codec libraries)
- Hardware acceleration coordination (delegated to NX-Core hardware abstraction)
- Memory management for video buffers (delegated to NX-Core memory management)
- File I/O operations (delegated to NX-Core I/O subsystem)

### Forbidden Responsibilities
- Creative video decisions or subjective quality assessments
- Automatic color grading or enhancement operations
- Content-aware processing or intelligent video optimization
- Streaming platform-specific optimizations that compromise accuracy
- User interface concerns or interactive processing controls

## 4. Explicit Video Pipeline Architecture

The video processing pipeline follows a strict seven-stage architecture where each stage performs a single, well-defined operation:

### Demux
**Purpose**: Separates video streams from container formats and validates stream integrity  
**Inputs**: Container files, stream selection parameters  
**Outputs**: Raw video streams with complete metadata preservation  
**Constraints**: Must preserve all timing information and metadata without modification  
**Bypass Conditions**: Never bypassed; always required for pipeline entry  
**Forbidden Behavior**: Stream reordering, metadata inference, or format assumptions

### Decode
**Purpose**: Converts encoded video frames to uncompressed format for processing  
**Inputs**: Encoded video streams, decoding parameters  
**Outputs**: Uncompressed video frames in native color space and resolution  
**Constraints**: Must preserve original color space, bit depth, and HDR metadata  
**Bypass Conditions**: Never bypassed; always required for processing chain  
**Forbidden Behavior**: Automatic color space conversion, range adjustment, or quality enhancement

### ColorSpaceNode
**Purpose**: Performs explicit color space transformations with mathematical precision  
**Inputs**: Video frames in source color space, target color space specifications  
**Outputs**: Video frames in target color space with preserved metadata  
**Constraints**: Must use standardized transformation matrices with documented precision  
**Bypass Conditions**: Bypassed when source and target color spaces are identical  
**Forbidden Behavior**: Silent color space conversion, automatic range changes, or HDR tone mapping

### ScaleNode
**Purpose**: Performs explicit spatial scaling using deterministic algorithms  
**Inputs**: Video frames at source resolution, target resolution specifications  
**Outputs**: Video frames at target resolution with preserved aspect ratio  
**Constraints**: Must use deterministic scaling algorithms with consistent filter characteristics  
**Bypass Conditions**: Bypassed when source and target resolutions are identical  
**Forbidden Behavior**: Automatic resolution selection, aspect ratio changes, or quality-based scaling decisions

### FrameQueue
**Purpose**: Manages frame ordering and ensures deterministic scheduling for encoding  
**Inputs**: Processed video frames, timing metadata, GOP structure requirements  
**Outputs**: Ordered frame sequences ready for encoding with consistent timing  
**Constraints**: Must maintain frame order determinism regardless of processing threading  
**Bypass Conditions**: Never bypassed; required for deterministic frame scheduling  
**Forbidden Behavior**: Frame reordering for optimization, timing adjustment, or quality improvement

### EncoderNode
**Purpose**: Encodes video frames using specified codec with explicit quality parameters  
**Inputs**: Uncompressed video frames, encoding specifications, quality parameters  
**Outputs**: Encoded video streams with documented quality characteristics  
**Constraints**: Must produce deterministic output regardless of hardware acceleration state  
**Bypass Conditions**: Never bypassed; always required for output generation  
**Forbidden Behavior**: Automatic quality adjustment, content-aware optimization, or non-deterministic encoding

### Mux
**Purpose**: Combines encoded video streams into target container format  
**Inputs**: Encoded video streams, container specifications, metadata requirements  
**Outputs**: Final video files with complete metadata and timing information  
**Constraints**: Must preserve all metadata and maintain timing accuracy  
**Bypass Conditions**: Never bypassed; always required for pipeline exit  
**Forbidden Behavior**: Metadata inference, timing adjustment, or format optimization

## 5. Color Management & Safety Model

NX-VideoTrans implements explicit handling of color primaries, transfer functions, and transformation matrices with no silent conversions between different color spaces. All color transformations require explicit user authorization with full disclosure of the transformation parameters and expected visual impact.

The system prevents silent BT.709 to BT.2020 conversions or any other color space changes that could affect the visual appearance of the content. Color space transformations are performed only when explicitly requested and use standardized transformation matrices with documented precision characteristics.

Range changes between full and limited range are never performed implicitly. All range conversions require explicit user authorization and are documented in processing reports with complete parameter disclosure. The system validates that range conversions are mathematically correct and preserve the intended visual appearance.

HDR metadata passthrough is implemented as a core safety feature, ensuring that HDR10, HDR10+, and Dolby Vision metadata is preserved without modification unless explicit metadata editing is requested. The system treats HDR metadata as immutable data that must be preserved to maintain proper display characteristics.

Color safety is treated as a non-negotiable invariant because color accuracy is fundamental to professional video workflows. A single undetected color shift can compromise the creative intent of the content, violate broadcast standards, and create inconsistencies across different display technologies and viewing environments.

## 6. Scaling & Spatial Processing

Spatial scaling operations are performed only when explicitly requested by the user with full specification of the target resolution and scaling algorithm. The system never performs automatic resizing for compatibility purposes or convenience features that could affect the visual quality of the content.

Deterministic scaling algorithms ensure that identical scaling operations produce bit-identical results across different processing environments and hardware configurations. All scaling operations use standardized filter characteristics with documented frequency response and aliasing performance.

Automatic resizing for compatibility is explicitly forbidden to prevent unintended quality degradation or aspect ratio changes that could affect the visual appearance of the content. All resolution changes must be explicitly authorized with full understanding of the visual impact.

Aspect ratio preservation rules ensure that scaling operations maintain the original aspect ratio unless explicitly overridden by the user. The system prevents accidental aspect ratio changes that could distort the visual content or create display compatibility issues.

Pixel format conversion constraints require that all format changes be explicitly specified and validated for compatibility with the target encoding requirements. The system prevents automatic pixel format conversions that could affect color accuracy or introduce processing artifacts.

## 7. Frame Queue & Deterministic Scheduling

The FrameQueue exists to ensure deterministic frame ordering and scheduling regardless of the threading model used for processing acceleration. It serves as a synchronization point that guarantees consistent GOP structure and frame timing across different processing environments.

Frame ordering determinism is enforced through strict sequencing rules that maintain the original frame order and timing relationships. The queue ensures that frames are presented to the encoder in the correct order regardless of the completion time of individual processing operations.

Threading constraints require that parallel processing operations do not affect the final frame order or timing characteristics. The FrameQueue isolates the deterministic requirements of encoding from the performance optimizations of parallel processing.

Frame reordering is never allowed to affect the output frame sequence, ensuring that the encoded video maintains the correct temporal relationships and GOP structure. Any reordering required by the encoder is handled internally within the encoding process without affecting the input frame sequence.

## 8. Encoder Abstraction Model

The encoder abstraction provides a unified interface across different hardware and software encoding implementations:

### VideoEncoder
**Common Interface Guarantees**: All encoders provide identical parameter interfaces and produce deterministic output given identical inputs and settings.

### CPUEncoder
**Implementation**: Software-based encoding using optimized CPU algorithms with deterministic behavior across different processor architectures.

### NVENC
**Implementation**: NVIDIA hardware-accelerated encoding with software fallback for deterministic output verification.

### QuickSync
**Implementation**: Intel hardware-accelerated encoding with software fallback for deterministic output verification.

### AMF
**Implementation**: AMD hardware-accelerated encoding with software fallback for deterministic output verification.

**Deterministic Parameter Mapping**: All encoder implementations use identical parameter mapping to ensure consistent behavior regardless of the underlying hardware or software implementation.

**Hardware Acceleration Without Output Variation**: Hardware-accelerated encoders must produce output that is functionally identical to software implementations, with any differences limited to compression efficiency rather than visual quality.

**Safe Software Fallback Requirements**: Every hardware encoder must have a software fallback that produces identical results, enabling deterministic behavior verification and ensuring operation on systems without specific hardware acceleration capabilities.

## 9. Encoder Capabilities

Each encoder exposes standardized capabilities through the common interface:

**Profiles**: Support for industry-standard encoding profiles including baseline, main, and high profiles with explicit parameter control and validation.

**GOP Structure**: Explicit control over Group of Pictures structure including I-frame intervals, B-frame patterns, and reference frame management with deterministic behavior.

**Bitrate Modes**: Support for Constant Bitrate (CBR), Variable Bitrate (VBR), and Constant Rate Factor (CRF) modes where applicable, with explicit parameter specification and validation.

**Look-ahead Support**: Configurable look-ahead analysis for encoders that support multi-pass encoding, with deterministic behavior and explicit parameter control.

**No Automatic Tuning**: The system never performs automatic parameter adjustment based on content analysis or performance considerations. All encoding parameters must be explicitly specified.

**No Content-Aware Optimization**: Encoders do not analyze content characteristics to automatically adjust encoding parameters or optimize for specific content types.

**Explicit Parameter Selection Only**: All encoding parameters must be explicitly specified by the user or automation scripts, with no automatic parameter inference or optimization.

## 10. Professional Targets & Presets

NX-VideoTrans provides specialized support for professional encoding targets that require stricter guarantees than consumer streaming formats:

**ProRes and DNxHR Mezzanine Formats**: Full support for professional mezzanine codecs with mathematically lossless or visually lossless compression, maintaining full color accuracy and metadata preservation for post-production workflows.

**Broadcast-Safe Encoding Presets**: Predefined encoding configurations that ensure compliance with broadcast standards including proper color space handling, GOP structure requirements, and bitrate constraints for different broadcast applications.

**Archive-Grade Intraframe Modes**: Specialized encoding modes designed for long-term archival storage with emphasis on error resilience, format longevity, and mathematical precision rather than compression efficiency.

These professional targets require stricter guarantees than streaming presets because they serve as intermediate formats in complex production workflows, must maintain quality through multiple generations of processing, and must comply with strict industry standards for broadcast and archival applications.

## 11. Determinism, Validation & Verification

Frame-level determinism guarantees ensure that identical encoding operations produce bit-identical results across different processing environments, hardware configurations, and execution times. This determinism is critical for professional workflows that require reproducible results for quality assurance and compliance verification.

Hashing and verification strategies include frame-level checksums, GOP structure validation, and bitstream analysis to ensure that encoding operations produce expected results. These verification methods enable detection of any non-deterministic behavior or processing errors.

Encoder output validation includes verification of GOP structure, bitrate compliance, color metadata preservation, and timing accuracy. All validation results are documented in comprehensive reports that enable quality assurance and troubleshooting.

Deterministic video encoding is critical for professional pipelines because it enables reliable batch processing, automated quality assurance, and reproducible results across different systems and time periods. Professional video workflows must be able to trust that identical encoding operations will produce identical results regardless of when or where they are executed.

## 12. Outputs & Verification Artifacts

NX-VideoTrans produces comprehensive outputs and verification artifacts for every encoding operation:

**Encoding Reports**: Complete documentation of all encoding parameters, processing decisions, and quality metrics. These reports enable full reconstruction of the encoding process and serve as audit trails for compliance verification.

**Frame Count and Timing Verification**: Detailed analysis of frame counts, timing relationships, and GOP structure to ensure that the encoded video maintains the correct temporal characteristics and complies with format requirements.

**Color Metadata Verification**: Documentation of color space preservation, HDR metadata handling, and range conversion operations to ensure that color characteristics are maintained correctly throughout the encoding process.

**Bitrate and GOP Validation**: Analysis of bitrate compliance, GOP structure accuracy, and encoding efficiency to verify that the encoded video meets specified quality and format requirements.

**Error and Warning Reporting**: Detailed logs of any issues encountered during encoding, including parameter conflicts, quality warnings, and compliance alerts. This reporting enables troubleshooting and validation of encoding results.

Each artifact provides specific information required for professional video workflows, enabling users to validate that encoding operations met their technical and quality requirements while maintaining compliance with industry standards.

## 13. Explicit Non-Goals

NX-VideoTrans deliberately excludes the following capabilities to maintain focus on accurate, transparent video encoding:

- **Color Grading**: No automatic color correction, white balance adjustment, or creative color enhancement operations that require subjective decisions about visual aesthetics.

- **HDR Tone Mapping**: No automatic conversion between HDR and SDR formats or tone mapping operations that could affect the creative intent of the content.

- **AI Upscaling or Enhancement**: No artificial intelligence-based upscaling, sharpening, or quality enhancement features that introduce non-deterministic processing or subjective quality improvements.

- **Content-Aware Bitrate Decisions**: No automatic bitrate adjustment based on content analysis, scene complexity, or motion characteristics. All bitrate parameters must be explicitly specified.

- **Streaming Platform Optimizations**: No automatic optimization for specific streaming platforms or delivery methods that could compromise color accuracy or introduce platform-specific processing artifacts.

- **UI-Driven Convenience Shortcuts**: No automatic parameter selection or processing shortcuts designed to simplify user interaction at the expense of explicit control and transparency.

- **Real-Time Processing Optimization**: No processing shortcuts or quality trade-offs designed to improve real-time performance at the expense of accuracy or determinism.

- **Automatic Format Selection**: No intelligent selection of encoding formats or parameters based on content analysis or delivery requirements. All format decisions must be explicitly specified by users or automation scripts.