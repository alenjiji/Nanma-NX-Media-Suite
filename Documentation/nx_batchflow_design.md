# NX-BatchFlow — Engineering Design Document

## 1. Component Overview

NX-BatchFlow is the intelligent batch processing and workflow automation engine within Nanma NX-MediaSuite, designed to scale all suite tools into deterministic, production-grade batch pipelines. It provides comprehensive workflow orchestration capabilities while maintaining the strict determinism and transparency requirements of professional media processing.

NX-BatchFlow exists to address the critical need for reliable, scalable automation in professional media workflows where hundreds or thousands of files must be processed consistently with identical results. Professional media environments require batch processing systems that can execute complex workflows reliably over extended periods without human intervention while maintaining complete auditability and reproducibility.

Batch-first design is critical for professional media workflows because interactive processing does not scale to production volumes, manual processing introduces human error and inconsistency, and professional deliverables require documented, repeatable processes for quality assurance and compliance verification. Batch processing enables lights-out operation, consistent quality control, and efficient resource utilization across large media libraries.

Typical use cases include broadcast pipeline automation for daily content processing, archival processing workflows for digitization and format migration, studio automation for repetitive post-production tasks, compliance processing for delivery requirement validation, and disaster recovery workflows for media asset restoration. Each use case demands absolute reliability and complete transparency in processing operations.

## 2. Design Intent & Execution Guarantees

NX-BatchFlow provides the following immutable guarantees for professional batch processing:

- **Deterministic Job Execution**: Given identical input files and job definitions, the system produces identical results across different executions, hardware configurations, and time periods. Execution order and processing decisions remain consistent and predictable.

- **Explicit Workflow Definition**: All workflow logic is explicitly defined in human-readable, version-controlled formats. No hidden automation logic, implicit dependencies, or undocumented processing decisions are permitted.

- **No Hidden Retries or Automatic Behavior**: All retry logic, failure handling, and recovery behavior is explicitly defined and documented. The system never performs automatic retries, error correction, or workflow modifications without explicit authorization.

- **Full Auditability of Batch Runs**: Every batch execution generates comprehensive audit trails documenting all processing decisions, execution timing, resource utilization, and results. Audit trails enable complete reconstruction of batch processing operations.

- **Identical Results Between Interactive and Batch Execution**: Batch processing operations produce identical results to their interactive counterparts, ensuring that batch automation does not introduce processing variations or quality differences.

## 3. Core Engine Responsibility

The `nx::batch::BatchEngine` serves as the central orchestrator for all batch processing operations within NX-BatchFlow, coordinating workflow execution while delegating actual media processing to specialized components.

### Engine Ownership
- Job graph construction, validation, and execution scheduling
- Workflow dependency resolution and execution order determination
- Resource allocation and concurrency management across processing operations
- Failure detection, retry coordination, and error propagation handling
- Audit trail generation and execution verification reporting

### Orchestration Responsibilities
- Coordination of NX-Convert Pro for transcoding operations
- Coordination of NX-AudioLab for audio processing workflows
- Coordination of NX-VideoTrans for video encoding operations
- Coordination of NX-MetaFix for metadata and container repair tasks
- Integration with external tools and validation systems as required

### Delegated Responsibilities
- All media processing operations (delegated to appropriate NX components)
- File I/O operations (delegated to NX-Core I/O subsystem)
- Hardware resource management (delegated to NX-Core hardware abstraction)
- Storage and backup operations (delegated to NX-Core storage management)

### Forbidden Responsibilities
- Direct media processing or content modification operations
- Creative decisions about processing parameters or quality settings
- Automatic workflow optimization or intelligent processing decisions
- User interface concerns or interactive processing controls
- Content analysis or adaptive processing behavior

## 4. Job Graph (DAG) Model

The job model is implemented as a Directed Acyclic Graph to ensure deterministic execution and clear dependency relationships:

### JobNode Structure
**Dependencies**: Explicit list of prerequisite jobs that must complete successfully before this job can execute, ensuring proper execution order and resource coordination  
**Retry Policy**: Explicit specification of retry behavior including maximum retry attempts, retry intervals, and conditions under which retries are attempted  
**Failure Strategy**: Defined behavior when job execution fails, including options to abort the entire graph, skip dependent branches, or continue execution with documented warnings

### DAG Rationale
Directed Acyclic Graphs are used because they provide clear dependency modeling without circular dependencies, enable parallel execution of independent jobs, support deterministic topological sorting for execution order, and allow efficient validation of workflow correctness before execution begins.

### Dependency Resolution
Dependencies are resolved through topological sorting that ensures all prerequisite jobs complete before dependent jobs begin execution. The resolution algorithm is deterministic and produces consistent execution orders given identical job graphs and dependency specifications.

### Cycle Prevention
Cycles are forbidden because they create undefined execution behavior, prevent deterministic completion guarantees, make failure recovery impossible to reason about, and violate the principle of explicit workflow definition. All job graphs are validated for acyclicity before execution begins.

### Execution Order Determinism
Determinism is preserved through consistent topological sorting algorithms, explicit priority specifications for jobs with equivalent dependencies, deterministic tie-breaking rules for parallel execution decisions, and documented execution order recording for audit and reproduction purposes.

## 5. JobNode Semantics

A JobNode represents a single, atomic unit of work within a batch workflow that corresponds to a specific operation performed by one of the NX-MediaSuite components. Jobs are designed to be independent, repeatable, and verifiable units that can be executed in isolation or as part of larger workflows.

Jobs map directly to NX component operations: NX-Convert Pro jobs handle format conversion and transcoding operations, NX-AudioLab jobs perform audio processing and analysis tasks, NX-VideoTrans jobs execute video encoding and processing workflows, and NX-MetaFix jobs manage metadata and container repair operations. Each job type maintains the same guarantees and behavior as its corresponding interactive component.

Job identity and immutability ensure that jobs are uniquely identifiable through content-based hashing, remain unchanged once defined within a workflow execution, can be safely cached and reused across different workflow runs, and maintain consistent behavior regardless of execution context or timing.

Input and output contracts define explicit specifications for required input files and parameters, guaranteed output files and verification artifacts, error conditions and failure modes, and resource requirements including CPU, memory, and storage needs. These contracts enable workflow validation before execution and ensure that job dependencies are properly satisfied.

## 6. Failure Handling & Retry Policies

The retry policy system provides explicit control over failure recovery behavior without hidden or automatic retry logic:

**Explicit Retry Policy Design**: All retry behavior is explicitly defined in job specifications including maximum retry attempts, retry interval timing, conditions that trigger retry attempts, and conditions that prevent retry attempts. No implicit retry logic is permitted.

**Deterministic Retry Behavior**: Retry operations follow deterministic patterns with consistent timing intervals, identical retry conditions across different executions, documented retry attempt logging, and predictable failure escalation behavior.

**Failure Propagation Rules**: Job failures propagate through the dependency graph according to explicit rules including immediate dependent job cancellation, optional branch skipping for non-critical dependencies, configurable failure isolation to prevent cascade failures, and documented failure impact assessment.

**Failure Strategy Options**: Available strategies include abort graph execution to stop all remaining jobs when critical failures occur, skip branch execution to continue processing unaffected workflow branches, and continue with warnings to complete workflow execution while documenting all failure conditions.

**Explicit Retry Requirements**: No implicit retries are allowed under any circumstances. All retry attempts must be explicitly authorized in job specifications, documented in execution logs, and subject to the same determinism requirements as initial job execution. Retry logic must be transparent and auditable.

## 7. Scheduler Architecture

The scheduler manages job execution while maintaining deterministic behavior and optimal resource utilization:

**CPU vs GPU Job Separation**: The scheduler maintains separate queues for CPU-intensive and GPU-accelerated jobs to prevent resource contention, optimize hardware utilization, enable independent scaling of different resource types, and maintain predictable execution timing for different job categories.

**Priority Queues**: Job prioritization uses explicit priority specifications from workflow definitions, deterministic priority resolution for jobs with identical priorities, fair scheduling algorithms that prevent priority inversion, and documented priority impact on execution order and resource allocation.

**Resource-Aware Execution**: The scheduler monitors and manages CPU core allocation, memory usage limits, GPU utilization and memory, storage I/O bandwidth, and network resources to ensure optimal performance without resource exhaustion or contention that could affect deterministic execution.

**Concurrency Limits**: Configurable limits control maximum concurrent jobs per resource type, maximum total system load, per-workflow concurrency restrictions, and resource reservation for critical system operations. These limits ensure stable operation and predictable performance characteristics.

**Fairness vs Throughput Trade-offs**: The scheduler balances fair resource allocation across different workflows, maximum system throughput optimization, priority-based resource allocation, and deterministic execution order preservation. Trade-off decisions are explicitly configured and documented.

Scheduling determinism is maintained through consistent job ordering algorithms, reproducible resource allocation decisions, documented scheduling policy enforcement, and elimination of timing-dependent scheduling variations that could affect workflow execution order or results.

## 8. Preset System

The preset system provides declarative workflow definition with version control and backward compatibility:

**Declarative Format**: Presets use structured YAML or JSON formats that are human-readable and editable, machine-parseable for validation and execution, version-controllable through standard source control systems, and diffable for change tracking and review processes.

**Versioned Presets**: All presets include explicit version identifiers, backward compatibility specifications, migration procedures for version updates, and deprecation policies for obsolete preset formats. Version management ensures long-term workflow stability and reproducibility.

**Human-Diffable Design**: Preset formats are designed for clear change visualization, meaningful diff output for code review processes, structured organization that highlights important changes, and consistent formatting that minimizes spurious differences between versions.

**Backward Compatibility Expectations**: Preset compatibility is maintained within major version releases, breaking changes require major version increments and migration documentation, deprecated features receive advance notice and migration guidance, and legacy preset support enables long-term workflow preservation.

**Preset Design Philosophy**: Presets are NOT UI snapshots or configuration dumps but are engineered workflow definitions that capture processing intent, maintain independence from UI implementation details, support programmatic generation and modification, and enable workflow sharing and collaboration across different environments.

**Job Graph Mapping**: Presets map to job graphs through declarative job specifications, explicit dependency definitions, clear parameter specifications, and deterministic graph construction algorithms that ensure consistent workflow execution regardless of preset loading environment.

**Preset Evolution**: Safe preset evolution is handled through semantic versioning, automated migration tools, validation of preset changes, and comprehensive testing of preset compatibility across different system versions.

## 9. Folder Watch & Automation

Folder-based automation provides reliable, deterministic file processing triggers:

**Hash-Based Change Detection**: File monitoring uses cryptographic hashing to detect actual file changes, eliminate false positives from timestamp modifications, ensure detection of content changes regardless of metadata, and provide reliable change verification for automation triggers.

**Atomic File Readiness Checks**: The system ensures files are completely written before processing through file locking verification, size stability monitoring over configurable time periods, hash stability confirmation to detect ongoing writes, and explicit ready-file markers where supported by workflow requirements.

**Partial File Prevention**: Processing of incomplete or unstable files is prevented through comprehensive readiness validation, configurable stability timeouts, file lock detection and waiting, and explicit file completion signaling mechanisms that ensure only complete, stable files enter processing workflows.

**Deterministic Triggering Rules**: Automation triggers follow explicit rules including file pattern matching specifications, directory monitoring scope definitions, processing delay configurations, and trigger condition documentation that ensures consistent automation behavior across different environments and time periods.

**Folder Watch Guarantees**: The system never guesses file readiness based on heuristics or assumptions, always validates file completeness before processing begins, documents all trigger decisions and timing in audit logs, and provides reproducible trigger behavior that can be verified and debugged when necessary.

## 10. Determinism, Validation & Auditability

Batch determinism enforcement ensures consistent, reproducible workflow execution:

**Batch Determinism Enforcement**: Deterministic behavior is maintained through consistent job ordering algorithms, reproducible resource allocation decisions, elimination of timing-dependent execution variations, and standardized processing parameter application across different execution environments.

**Execution Order Recording**: All workflow executions document complete job execution sequences, timing information for each processing stage, resource utilization throughout execution, and dependency resolution decisions that enable exact reproduction of workflow execution patterns.

**Output Verification**: Batch outputs are verified through comprehensive result validation, comparison with expected output specifications, integrity checking of all generated files, and verification that processing objectives were achieved according to workflow specifications.

**Long-Term Reproducibility**: Batch runs remain reproducible months or years later through complete parameter preservation, environment specification documentation, dependency version recording, and archived execution context that enables exact reproduction of historical processing operations.

## 11. Outputs & Execution Artifacts

NX-BatchFlow produces comprehensive outputs and execution artifacts for every batch operation:

**Batch Execution Reports**: Complete documentation of workflow execution including job completion status, processing timing and performance metrics, resource utilization throughout execution, and overall workflow success or failure assessment with detailed analysis.

**Job-Level Logs**: Detailed logging for individual jobs including processing parameters and configuration, input and output file information, processing duration and resource usage, and any warnings or errors encountered during job execution.

**Failure Summaries**: Comprehensive analysis of any failures including root cause identification, failure impact assessment, recovery recommendations, and prevention strategies for avoiding similar failures in future workflow executions.

**Retry Records**: Complete documentation of all retry attempts including retry trigger conditions, retry attempt timing and results, retry success or failure outcomes, and cumulative retry impact on overall workflow execution timing and resource usage.

**Execution Timelines**: Detailed timeline documentation including job start and completion times, dependency resolution timing, resource allocation and release events, and overall workflow execution progression that enables performance analysis and optimization.

These artifacts serve critical professional workflow requirements including compliance documentation for audit and regulatory purposes, troubleshooting and optimization guidance for workflow improvement, quality assurance verification for processing validation, and historical analysis for capacity planning and performance optimization.

## 12. Explicit Non-Goals

NX-BatchFlow deliberately excludes the following capabilities to maintain focus on reliable workflow orchestration:

- **Media Processing**: No direct processing, modification, or analysis of audio or video content. All media operations are delegated to specialized NX components designed for specific processing requirements.

- **Intelligent or Adaptive Workflow Changes**: No automatic workflow modification based on content analysis, processing results, or environmental conditions. All workflow logic must be explicitly defined and remain consistent across executions.

- **Content-Aware Decision Making**: No processing decisions based on analysis of media content characteristics, quality assessment, or content classification. All processing parameters must be explicitly specified in workflow definitions.

- **UI-Driven Automation Logic**: No automation behavior that depends on user interface state, interactive settings, or GUI-specific configuration. All automation must be headless-capable and independent of user interface implementations.

- **Hidden Optimizations or Shortcuts**: No automatic optimization of workflow execution, processing shortcuts based on content analysis, or performance trade-offs that could affect processing determinism or output quality.

- **Real-Time Processing Adaptation**: No dynamic adjustment of processing parameters based on system performance, resource availability, or processing queue status that could affect workflow determinism or reproducibility.

- **Automatic Error Recovery**: No intelligent error recovery or automatic workflow repair that could mask processing problems or introduce non-deterministic behavior into workflow execution.