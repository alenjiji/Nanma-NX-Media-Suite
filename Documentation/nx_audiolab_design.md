# NX-AudioLab — Engineering Design Document

## 1. Component Overview

NX-AudioLab is the high-resolution audio engine within Nanma NX-MediaSuite, designed for sample-accurate audio processing in professional mastering, broadcast preparation, and quality control workflows. It provides deterministic digital signal processing capabilities with explicit control over every audio transformation.

NX-AudioLab exists to address the specialized requirements of professional audio workflows that demand mathematical precision, deterministic behavior, and complete transparency in audio processing operations. While NX-Convert Pro handles format conversion and basic transcoding, NX-AudioLab focuses on the precise audio processing operations required for broadcast compliance, mastering preparation, and quality assurance.

The component is designed to solve audio processing problems that require sample-level accuracy, including loudness measurement and normalization for broadcast standards, sample rate conversion with controlled aliasing characteristics, bit-depth management with proper dithering, and gain adjustment with clipping prevention. These operations require specialized DSP algorithms and cannot be safely performed by general-purpose transcoding tools.

NX-AudioLab is separated from basic transcoding functionality because audio processing requires different guarantees, validation methods, and safety constraints than format conversion. Professional audio workflows demand mathematical correctness, deterministic behavior across different hardware platforms, and explicit control over every processing parameter that could affect audio quality.

## 2. Design Intent & Professional Guarantees

NX-AudioLab provides the following immutable guarantees for professional audio processing:

- **Sample-Accurate Processing**: All operations maintain sample-level precision with no timing drift, phase shifts, or synchronization errors across processing operations.

- **Deterministic DSP Behavior**: Given identical inputs and parameters, the system produces bit-identical audio outputs across different executions, hardware configurations, and processing environments.

- **Explicit Processing Graphs**: Every audio transformation is represented as a visible, auditable processing node. No hidden operations, automatic corrections, or implicit processing steps are performed.

- **Zero Hidden Gain, SRC, or Normalization**: The system never applies automatic gain control, sample rate conversion, or loudness normalization without explicit user authorization and complete parameter disclosure.

- **Broadcast-Safe Correctness**: All processing operations maintain compliance with professional broadcast standards, including proper handling of true peak detection, loudness measurement, and clipping prevention.

- **Mathematical Precision**: Internal processing uses 64-bit floating-point arithmetic to prevent accumulation of quantization errors during complex processing chains.

## 3. Core Engine Responsibility

The `nx::audio::AudioEngine` serves as the central coordinator for all audio processing operations within NX-AudioLab, maintaining strict adherence to professional audio processing requirements.

### Engine Ownership
- Audio processing graph construction and validation
- DSP algorithm coordination and execution
- Sample-accurate timing and synchronization
- Processing parameter validation and safety checking
- Audio quality verification and measurement reporting

### Delegated Responsibilities
- Audio format decoding and encoding (delegated to NX-Core codec libraries)
- File I/O operations (delegated to NX-Core I/O subsystem)
- Hardware acceleration coordination (delegated to NX-Core hardware abstraction)
- Memory management for audio buffers (delegated to NX-Core memory management)

### Forbidden Responsibilities
- Creative audio decisions or subjective quality assessments
- Automatic mastering operations or dynamic range processing
- Content-aware processing or intelligent audio enhancement
- Format conversion operations handled by NX-Convert Pro
- User interface concerns or interactive processing controls

## 4. Internal Processing Graph

The internal processing graph follows a strict six-node architecture where each node is included only when explicitly required:

### DecodeNode
**Purpose**: Converts encoded audio to internal 64-bit floating-point format  
**Inputs**: Encoded audio streams, decoding parameters  
**Outputs**: 64-bit floating-point audio samples with preserved metadata  
**Constraints**: Must preserve original bit depth and sample rate information in metadata  
**Bypass Conditions**: Never bypassed; always required for processing chain entry  
**Forbidden Behavior**: Sample rate conversion, gain adjustment, or any processing beyond format conversion

### SRCNode
**Purpose**: Performs sample rate conversion using polyphase linear-phase filtering  
**Inputs**: Audio samples at source sample rate, target sample rate specification  
**Outputs**: Audio samples at target sample rate with controlled aliasing characteristics  
**Constraints**: Must use deterministic filtering with configurable stopband attenuation  
**Bypass Conditions**: Bypassed when source and target sample rates are identical  
**Forbidden Behavior**: Automatic sample rate selection, quality trade-offs, or implicit conversion

### GainNode
**Purpose**: Applies precise gain adjustment with clipping prevention  
**Inputs**: Audio samples, gain specification in dB, clipping prevention parameters  
**Outputs**: Gain-adjusted audio samples with clipping protection  
**Constraints**: Must prevent clipping unless explicitly authorized by user  
**Bypass Conditions**: Bypassed when gain adjustment is 0 dB and no clipping protection is required  
**Forbidden Behavior**: Automatic gain adjustment, dynamic range compression, or loudness-based gain changes

### LoudnessNode
**Purpose**: Measures and optionally normalizes audio loudness according to ITU-R BS.1770  
**Inputs**: Audio samples, loudness measurement parameters, optional normalization targets  
**Outputs**: Audio samples (unchanged in measure-only mode), loudness measurement data  
**Constraints**: Must operate in measure-only mode unless normalization is explicitly authorized  
**Bypass Conditions**: Bypassed when no loudness measurement or normalization is required  
**Forbidden Behavior**: Automatic normalization, content-aware loudness adjustment, or implicit loudness correction

### DitherNode
**Purpose**: Applies dithering and noise shaping when reducing bit depth  
**Inputs**: 64-bit floating-point audio samples, target bit depth, dithering parameters  
**Outputs**: Dithered audio samples at target bit depth  
**Constraints**: Must use deterministic noise generation with seeded random number generators  
**Bypass Conditions**: Bypassed when target bit depth equals or exceeds source bit depth  
**Forbidden Behavior**: Automatic dithering selection, quality-based dithering decisions, or non-deterministic noise generation

### EncodeNode
**Purpose**: Converts processed audio to target encoded format  
**Inputs**: Processed 64-bit floating-point audio samples, encoding specifications  
**Outputs**: Encoded audio stream in target format  
**Constraints**: Must preserve all processing metadata and maintain encoding determinism  
**Bypass Conditions**: Never bypassed; always required for processing chain exit  
**Forbidden Behavior**: Format optimization, automatic quality selection, or encoding parameter inference

## 5. Internal Audio Format & Precision Model

NX-AudioLab uses 64-bit floating-point format internally to provide sufficient precision for complex processing chains without accumulation of quantization errors. This format ensures that multiple processing operations can be chained together without degrading audio quality through repeated quantization.

Original format characteristics including bit depth, sample rate, and channel layout are preserved in comprehensive metadata that travels with the audio throughout the processing chain. This metadata enables proper reconstruction of the original format characteristics and ensures that processing decisions can be made with full knowledge of the source material properties.

Precision loss is prevented through careful management of numerical precision at each processing stage. All intermediate calculations use extended precision arithmetic where necessary, and quantization is performed only at the final output stage when converting to the target format. This approach ensures that the cumulative effect of processing operations does not introduce audible artifacts.

This precision model is critical for professional workflows because it enables complex processing chains to be constructed without concern for cumulative quality degradation. Professional audio applications often require multiple processing stages, and each stage must maintain the full fidelity of the source material to ensure that the final output meets professional quality standards.

## 6. Sample Rate Conversion (SRC)

The SRC implementation uses polyphase linear-phase filter design to ensure optimal frequency response characteristics and minimal phase distortion. The filter design employs configurable stopband attenuation parameters that allow users to specify the trade-off between filter complexity and aliasing rejection performance.

Deterministic behavior is ensured through fixed-point filter coefficient representation and consistent rounding behavior across different hardware platforms. All filter calculations use identical algorithms and precision regardless of the underlying processor architecture or optimization capabilities.

SRC is explicitly enabled only when the source and target sample rates differ and the user has specifically requested sample rate conversion. The system never performs implicit sample rate conversion, even when it might be convenient for format compatibility or processing efficiency.

Explicit enable/disable rules require that all sample rate conversion operations be specifically authorized by the user with full disclosure of the conversion parameters, including filter characteristics, stopband attenuation, and expected frequency response. This ensures that users understand the implications of sample rate conversion on their audio material.

## 7. Loudness Processing

Loudness processing strictly adheres to ITU-R BS.1770 standard for loudness measurement, ensuring compatibility with broadcast and streaming platform requirements. The implementation provides both measurement and normalization capabilities with clear distinction between these operations.

The system operates in measure-only mode by default, providing comprehensive loudness analysis without modifying the audio signal. This approach allows users to assess loudness characteristics before deciding whether normalization is necessary or appropriate for their specific application.

Normalization operations require explicit user authorization with full disclosure of the target loudness level, gating parameters, and expected gain adjustment. The system never performs automatic loudness normalization based on content analysis or format requirements.

True peak detection uses oversampled analysis to accurately identify intersample peaks that could cause clipping in downstream processing or playback systems. This detection is performed using standardized algorithms that ensure consistent results across different processing environments and hardware configurations.

## 8. Dithering & Bit-Depth Management

Dithering is applied only when the target bit depth is lower than the source bit depth or internal processing precision. The system never applies dithering when it would not provide benefit, ensuring that unnecessary noise is not introduced into the audio signal.

Dithering is explicitly forbidden unless bit depth reduction is occurring, preventing the introduction of noise into audio signals that do not require quantization. This policy ensures that audio quality is never degraded through unnecessary processing operations.

Noise shaping options include triangular probability density function dithering and various noise shaping curves optimized for different bit depths and applications. All noise shaping algorithms use deterministic implementations that produce identical results across different executions.

Deterministic noise generation uses seeded pseudorandom number generators with documented algorithms and initialization procedures. This ensures that dithering operations produce identical results when processing identical source material with identical parameters, enabling reproducible processing workflows.

## 9. Determinism, Validation & Safety

Deterministic DSP is enforced through strict control of all processing variables that could affect audio output. All algorithms use fixed-point arithmetic where possible, and floating-point operations use consistent rounding modes and precision settings across different hardware platforms.

Clipping prevention rules require that all processing operations monitor signal levels and prevent clipping unless explicitly authorized by the user. The system uses oversampled true peak detection to identify potential clipping conditions before they occur in the final output.

Explicit clipping authorization is required when processing operations could result in signal levels exceeding full scale. Users must acknowledge the potential for clipping and specify whether clipping should be prevented through gain reduction or allowed to occur with appropriate warning documentation.

Oversampled true peak detection operates at 4x oversampling to accurately identify intersample peaks that could cause clipping in digital-to-analog conversion or subsequent processing stages. This detection uses standardized algorithms that ensure consistent results across different processing environments.

Output validation includes verification of signal levels, frequency response characteristics, and compliance with specified processing parameters. All validation results are documented in comprehensive processing reports that enable quality assurance and troubleshooting.

## 10. Outputs & Verification Artifacts

NX-AudioLab produces comprehensive outputs and verification artifacts for every audio processing operation:

**Processing Reports**: Complete documentation of all processing operations performed, including node configurations, parameter settings, and processing order. These reports enable full reconstruction of the processing chain and serve as audit trails for quality assurance.

**Loudness Measurement Results**: Detailed loudness analysis including integrated loudness, loudness range, maximum momentary loudness, and maximum short-term loudness according to ITU-R BS.1770. Results include gating information and compliance assessment for various broadcast standards.

**Peak and True-Peak Reports**: Comprehensive analysis of signal levels including sample peak values, true peak measurements using oversampled analysis, and headroom calculations. Reports identify potential clipping conditions and provide recommendations for gain adjustment.

**Bit-Depth and Sample-Rate Verification**: Documentation of format preservation and conversion operations, including verification that bit depth and sample rate changes were performed as specified. This verification ensures that format requirements are met correctly.

**Error and Warning Reporting**: Detailed logs of any issues encountered during processing, including signal level warnings, processing parameter conflicts, and quality assurance alerts. This reporting enables troubleshooting and validation of processing results.

Each artifact provides specific information required for professional audio workflows, enabling users to validate that processing operations met their technical and quality requirements while maintaining compliance with industry standards.

## 11. Explicit Non-Goals

NX-AudioLab deliberately excludes the following capabilities to maintain focus on precise, transparent audio processing:

- **Creative Audio Mastering**: No automatic equalization, dynamic range compression, stereo enhancement, or other creative processing operations that require subjective decisions about audio aesthetics.

- **Automatic EQ, Compression, or Enhancement**: No content-aware processing that automatically adjusts frequency response, dynamic range, or other audio characteristics based on analysis of the source material.

- **Content-Aware Processing**: No intelligent processing that adapts behavior based on music genre, speech detection, or other content analysis. All processing operations must be explicitly specified and deterministic.

- **Implicit Normalization**: No automatic adjustment of audio levels based on format requirements, platform specifications, or content analysis. All normalization operations must be explicitly authorized with full parameter disclosure.

- **Format Conversion Responsibilities**: No handling of container formats, codec selection, or file format conversion operations that are properly handled by NX-Convert Pro. Audio processing operations are distinct from format conversion requirements.

- **Real-Time Processing Optimization**: No processing shortcuts or quality trade-offs designed to improve real-time performance at the expense of accuracy or determinism. All operations prioritize correctness over processing speed.

- **Automatic Parameter Selection**: No intelligent selection of processing parameters based on content analysis or format requirements. All processing parameters must be explicitly specified by users or automation scripts.