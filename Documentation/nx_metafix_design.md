# NX-MetaFix — Engineering Design Document

## 1. Component Overview

NX-MetaFix is the metadata, tagging, and container repair engine within Nanma NX-MediaSuite, designed to repair, validate, and normalize media containers and metadata without touching media essence. It provides deterministic container repair and metadata management capabilities for professional media workflows.

NX-MetaFix exists to address the critical need for container and metadata repair in professional media environments where files may become corrupted, incomplete, or non-compliant due to hardware failures, software errors, or incomplete transfers. Professional media workflows require tools that can restore structural integrity without risking the underlying audio and video content.

Metadata and container repair require different safety rules than transcoding because they operate on structural and descriptive information rather than media content. Container repair focuses on restoring file structure and indexing information, while metadata operations manage descriptive, technical, and rights information. These operations must guarantee that the actual media essence remains completely untouched.

Typical professional use cases include archival recovery where container headers have been damaged, broadcast preparation requiring metadata compliance validation, recovery operations for corrupted files with intact media streams, metadata normalization for asset management systems, and compliance verification for delivery requirements. Each use case demands absolute confidence that repair operations will not affect media quality or content.

## 2. Design Intent & Safety Guarantees

NX-MetaFix provides the following immutable guarantees for professional metadata and container operations:

- **Media Essence Immutability by Default**: All operations treat audio and video streams as read-only data that must never be modified unless explicitly overridden with full authorization and audit logging.

- **Deterministic Container Repair**: Given identical damaged containers and repair parameters, the system produces bit-identical repaired containers across different executions and hardware configurations.

- **Explicit Metadata Operations**: Every metadata modification is explicitly authorized and documented. No automatic metadata inference, enhancement, or correction is performed without user authorization.

- **Zero Hidden Stream Modification**: The system never modifies, re-encodes, or processes media streams during container repair or metadata operations. Stream data remains bit-identical to the source.

- **Auditability and Reversibility**: All operations generate comprehensive audit trails documenting exactly what was modified. Where possible, operations are designed to be reversible through backup and restoration procedures.

## 3. Core Engine Responsibility

The `nx::meta::MetaEngine` serves as the central coordinator for all metadata and container repair operations within NX-MetaFix, maintaining strict separation between structural operations and media content.

### Engine Ownership
- Container structure analysis and repair coordination
- Metadata graph construction and validation
- Repair operation sequencing and safety verification
- Metadata integrity checking and conflict resolution
- Audit trail generation and verification reporting

### Delegated Responsibilities
- Container format parsing (delegated to NX-Core format handlers)
- File I/O operations (delegated to NX-Core I/O subsystem)
- Metadata format handling (delegated to NX-Core metadata libraries)
- Backup and recovery operations (delegated to NX-Core storage management)

### Forbidden Responsibilities
- Media stream re-encoding or processing operations
- Creative metadata authoring or content inference
- Automatic metadata enhancement or quality improvement
- Format conversion operations handled by NX-Convert Pro
- User interface concerns or interactive editing features

## 4. Container Repair Architecture

Container repair capabilities focus on structural restoration without affecting media essence:

### Header Rebuild
**Purpose**: Reconstructs damaged or missing container headers using stream analysis and format specifications  
**Modified Data**: Container header structures, format identification atoms, and structural metadata  
**Untouched Data**: All media stream data, timing information, and stream-specific metadata  
**Deterministic Guarantees**: Identical source containers produce identical rebuilt headers using standardized format specifications  
**Validation Steps**: Header structure verification, format compliance checking, and stream accessibility validation

### Duration Index Regeneration
**Purpose**: Rebuilds duration and seeking index information for containers with damaged or missing indices  
**Modified Data**: Duration metadata, seeking tables, and frame index structures  
**Untouched Data**: Media stream content, frame data, and stream encoding parameters  
**Deterministic Guarantees**: Index regeneration produces identical results based on actual stream analysis rather than estimation  
**Validation Steps**: Seeking accuracy verification, duration calculation validation, and index structure integrity checking

### Atom / Table Repair
**Purpose**: Repairs or reconstructs specific container atoms or tables that are corrupted or inconsistent  
**Modified Data**: Specific container atoms, table structures, and cross-reference information  
**Untouched Data**: Media data atoms, stream content, and unrelated container structures  
**Deterministic Guarantees**: Atom repair follows standardized format specifications with consistent reconstruction algorithms  
**Validation Steps**: Atom structure validation, cross-reference integrity checking, and format compliance verification

### Interleave Correction
**Purpose**: Corrects container interleaving patterns to optimize playback performance without re-encoding streams  
**Modified Data**: Container structure organization and stream interleaving patterns  
**Untouched Data**: Stream content, frame boundaries, and media data integrity  
**Deterministic Guarantees**: Interleave correction produces consistent patterns based on standardized optimization algorithms  
**Validation Steps**: Interleave pattern verification, playback performance validation, and stream integrity confirmation

Container repair operations are structural rather than semantic, focusing on restoring technical functionality without making creative or interpretive decisions about content organization or metadata meaning.

## 5. Metadata Graph Model

The metadata model is structured as a hierarchical graph to ensure proper organization and integrity:

### MetadataRoot
**Responsibility**: Serves as the root node coordinating all metadata categories and maintaining overall metadata integrity and consistency across the entire graph structure.

### Technical
**Responsibility**: Manages technical metadata including codec information, resolution specifications, frame rates, bit depths, color space definitions, and other measurable technical characteristics of the media content.

### Descriptive
**Responsibility**: Handles descriptive metadata such as titles, descriptions, keywords, genre classifications, and other human-readable information that describes the content's meaning or context.

### Rights
**Responsibility**: Manages rights and licensing metadata including copyright information, usage restrictions, licensing terms, and other legal or commercial metadata related to content ownership and usage.

### Chapters
**Responsibility**: Coordinates chapter and navigation metadata including chapter titles, timestamps, navigation markers, and other structural information for content organization and user navigation.

### Artwork
**Responsibility**: Manages associated artwork and visual metadata including thumbnails, cover art, poster images, and other visual elements associated with the content but separate from the main media streams.

Metadata is modeled as a graph to represent complex relationships between different metadata elements while maintaining clear separation of concerns. This structure enables independent validation and modification of different metadata categories without affecting others.

The separation between technical and descriptive metadata ensures that measurable technical characteristics are handled differently from subjective descriptive information, preventing automatic inference or modification of creative metadata based on technical analysis.

Metadata integrity is preserved through graph validation that ensures consistency between related metadata elements and prevents orphaned or contradictory metadata relationships.

## 6. Metadata Operations

NX-MetaFix supports the following metadata operations with strict safety and determinism guarantees:

### Safe Metadata Merge
**Allowed Behavior**: Combines metadata from multiple sources using explicit precedence rules and conflict resolution strategies specified by the user  
**Forbidden Behavior**: Automatic metadata inference, creative decision-making about metadata conflicts, or silent overwriting of existing metadata without explicit authorization  
**Conflict Resolution Rules**: User-defined precedence rules determine which metadata takes priority when conflicts occur, with all conflicts documented in operation reports  
**Determinism Guarantees**: Identical source metadata and merge rules produce identical merged results across different executions

### Copy Metadata Between Containers
**Allowed Behavior**: Transfers specified metadata categories between compatible container formats while preserving metadata structure and relationships  
**Forbidden Behavior**: Automatic metadata format conversion, inference of missing metadata, or modification of metadata content during transfer operations  
**Conflict Resolution Rules**: Existing metadata in target containers is preserved unless explicitly overridden, with all overwrites requiring explicit authorization  
**Determinism Guarantees**: Metadata copying operations produce identical results given identical source and target containers and copy specifications

### Metadata Normalization
**Allowed Behavior**: Standardizes metadata format and structure according to specified schemas while preserving all semantic content and meaning  
**Forbidden Behavior**: Content modification, automatic enhancement, or inference of missing metadata elements based on existing metadata or content analysis  
**Conflict Resolution Rules**: Normalization conflicts are resolved using explicit mapping rules with all transformations documented and reversible where possible  
**Determinism Guarantees**: Normalization operations produce identical results when applied to identical source metadata using identical normalization rules

### Metadata Validation
**Allowed Behavior**: Verifies metadata completeness, format compliance, and internal consistency according to specified validation rules and industry standards  
**Forbidden Behavior**: Automatic correction of validation errors, inference of missing required metadata, or modification of metadata to achieve compliance  
**Conflict Resolution Rules**: Validation errors are reported with specific recommendations but no automatic corrections are applied without explicit user authorization  
**Determinism Guarantees**: Validation operations produce identical results and reports when applied to identical metadata using identical validation criteria

## 7. Broadcast & Compliance Validation

NX-MetaFix provides comprehensive validation capabilities for broadcast and compliance requirements:

**Broadcast Compatibility Checks**: Verification that container formats, metadata structures, and technical specifications comply with broadcast delivery requirements including format constraints, metadata completeness, and technical parameter validation.

**Container Compliance Validation**: Analysis of container structure integrity, format specification adherence, and compatibility with target playback and delivery systems including validation of required atoms, tables, and structural elements.

**Metadata Completeness Verification**: Assessment of metadata completeness according to specified requirements including verification of required fields, format compliance, and relationship consistency between different metadata categories.

**Error and Warning Classification**: Systematic categorization of validation issues including critical errors that prevent delivery, warnings that indicate potential compatibility issues, and informational notices about optimization opportunities.

All validation operations are read-only and never modify media content or container structures. Validation results provide detailed reports that enable users to understand compliance status and make informed decisions about necessary corrections.

## 8. Media Essence Protection Rules

The core rule governing all NX-MetaFix operations is:

**"Media essence is read-only unless explicitly overridden."**

**Media Essence Definition**: Media essence includes all audio and video stream data, frame content, sample data, and any information that directly affects the perceived quality or content of the media. This includes compressed stream data, timing information that affects playback, and stream-specific metadata that affects decoding.

**Override Authorization**: Overrides to the read-only rule require explicit user authorization with full disclosure of the operations that will affect media essence. Users must acknowledge the potential impact and provide explicit consent for any operations that could modify stream data.

**Override Logging and Auditing**: All overrides are comprehensively logged with complete documentation of what media essence was modified, why the modification was necessary, and what the expected impact will be. Audit trails enable full reconstruction of modification decisions.

**Professional Trust Rationale**: This rule is critical for professional trust because media essence represents the core value of media assets. Any unintended modification could compromise content quality, violate creative intent, or introduce artifacts that affect professional deliverables. The read-only default ensures that container and metadata operations never risk the fundamental media content.

## 9. Determinism, Validation & Auditability

Deterministic repair behavior ensures that identical repair operations produce identical results across different processing environments and execution times. All repair algorithms use standardized procedures with consistent decision-making logic that eliminates variability in repair outcomes.

Structural validation after operations includes comprehensive verification that repaired containers maintain format compliance, structural integrity, and compatibility with target playback systems. Validation confirms that repair operations achieved their intended goals without introducing new structural problems.

Metadata integrity verification ensures that metadata operations preserve semantic meaning, maintain relationship consistency, and comply with format requirements. All metadata modifications are validated to confirm that they achieve intended results without corrupting metadata structures.

Audit trail generation provides complete documentation of all operations performed, including input conditions, operation parameters, modification details, and validation results. Audit trails enable full reconstruction of repair and modification decisions for quality assurance and compliance verification.

## 10. Outputs & Verification Artifacts

NX-MetaFix produces comprehensive outputs and verification artifacts for every operation:

**Repair Reports**: Complete documentation of all repair operations performed including structural modifications, metadata changes, and validation results. Reports enable full understanding of what was repaired and verification that repairs achieved intended goals.

**Validation Summaries**: Comprehensive analysis of container and metadata compliance including identification of issues, assessment of severity, and recommendations for resolution. Summaries provide actionable information for compliance and quality assurance.

**Metadata Diffs**: Detailed comparison reports showing exactly what metadata was modified, added, or removed during operations. Diffs enable precise understanding of metadata changes and support rollback procedures where necessary.

**Compliance Warnings**: Systematic reporting of compliance issues including broadcast compatibility problems, format specification violations, and delivery requirement conflicts. Warnings provide specific guidance for achieving compliance.

**Error Reports**: Detailed documentation of any errors encountered during operations including root cause analysis, impact assessment, and recommended resolution procedures. Error reports enable troubleshooting and process improvement.

Each artifact serves specific professional workflow requirements, enabling users to validate that operations met their technical and compliance requirements while maintaining complete transparency about all modifications performed.

## 11. Explicit Non-Goals

NX-MetaFix deliberately excludes the following capabilities to maintain focus on safe container and metadata operations:

- **Media Re-encoding**: No processing, modification, or re-encoding of audio or video streams. All media essence operations are handled by specialized components designed for media processing.

- **Audio or Video Processing**: No signal processing, quality enhancement, or content modification operations that affect the actual media content rather than container or metadata structures.

- **Creative Metadata Authoring**: No automatic generation of creative or subjective metadata such as descriptions, keywords, or artistic classifications that require human judgment or creative input.

- **Content Inference**: No analysis of media content to automatically generate or modify metadata based on content characteristics, scene analysis, or audio/video feature detection.

- **Automatic Metadata Enhancement**: No automatic improvement, correction, or optimization of metadata content based on analysis of existing metadata or content characteristics.

- **Format Conversion**: No container format conversion or transcoding operations that are properly handled by NX-Convert Pro. Metadata operations are distinct from format conversion requirements.

- **Real-Time Processing**: No optimization for real-time or interactive processing that could compromise the accuracy or determinism of repair and metadata operations.

- **Subjective Quality Assessment**: No evaluation or modification of metadata based on subjective quality criteria or aesthetic considerations that require human judgment.