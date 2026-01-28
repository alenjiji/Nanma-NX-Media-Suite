# Phase 15.4-B — CI Enforcement Rules

Status: FROZEN (Phase 15 Architectural Contract)
Scope: CI Policy Specification
Authority: CLI Canonical Contract

# CI Enforcement Rules

## 1. Enforcement Scope

CI enforcement SHALL apply to all components within the Qt UI layer including:
- Qt widget implementations and their associated source files
- Qt adapter implementations that bridge CLI to UI components
- Qt application framework integration and windowing system code
- Qt-specific configuration files and UI state management code
- All build targets that produce Qt-dependent executables or libraries

CI enforcement SHALL NOT apply to:
- CLI executable implementations and core engine components
- Non-Qt adapter implementations and CLI bridge components
- Core system libraries and CLI-accessible functionality
- Configuration files readable by CLI commands
- Build targets that produce CLI-only executables

## 2. Forbidden Dependency Rules

### Illegal Include Detection
CI MUST reject any Qt UI component that includes:
- Core engine headers directly without adapter mediation
- CLI implementation headers or internal CLI interfaces
- System-level headers that bypass CLI command interfaces
- Non-Qt libraries that provide CLI-equivalent functionality
- Headers that expose business logic or execution semantics

### Illegal Linkage Detection
CI MUST reject any Qt UI component that links against:
- Core engine libraries directly without adapter isolation
- CLI implementation libraries or internal CLI modules
- System libraries that duplicate CLI-accessible functionality
- Non-Qt libraries that provide semantic processing capabilities
- Libraries that contain business logic or execution control

### Illegal Symbol Usage Detection
CI MUST reject any Qt UI component that references:
- Core engine symbols directly without adapter interfaces
- CLI internal symbols or implementation-specific identifiers
- System symbols that bypass CLI command processing
- Function symbols that perform semantic interpretation or validation
- Data symbols that represent authoritative system state

## 3. Adapter Purity Enforcement

### Qt-Free Adapter Requirements
CI MUST reject any adapter implementation that:
- Includes Qt headers or uses Qt-specific data types
- Links against Qt libraries or Qt framework components
- Uses Qt threading models or Qt event processing mechanisms
- Depends on Qt application lifecycle or windowing system integration
- Contains Qt-specific error handling or resource management patterns

### Stateless Adapter Requirements
CI MUST reject any adapter implementation that:
- Maintains persistent state between CLI command invocations
- Caches CLI results for reuse across multiple operations
- Stores configuration data or user preferences internally
- Implements session management or operation history tracking
- Contains static variables or global state that persists across calls

### Semantic Processing Prohibitions
CI MUST reject any adapter implementation that:
- Provides default values for missing CLI parameters
- Implements retry logic or automatic error recovery mechanisms
- Performs semantic validation beyond CLI parameter format checking
- Infers user intent or provides intelligent parameter completion
- Transforms CLI output data or adds interpretive metadata

## 4. CLI Contract Fidelity

### One-to-One Mapping Requirements
CI MUST reject any adapter implementation that:
- Combines multiple CLI commands into single adapter operations
- Splits single CLI commands across multiple adapter methods
- Provides adapter-specific functionality not available through CLI
- Implements convenience methods that bypass CLI parameter validation
- Creates composite operations that aggregate multiple CLI results

### CLI Bypass Prevention
CI MUST reject any UI component that:
- Accesses core functionality directly without CLI command invocation
- Implements business logic that duplicates CLI-available operations
- Provides alternative execution paths that bypass CLI validation
- Creates UI-specific workflows not available through CLI commands
- Maintains authoritative data that differs from CLI-accessible information

### Behavior Divergence Detection
CI MUST reject any UI implementation that:
- Produces different results than equivalent CLI command execution
- Implements different parameter validation rules than CLI commands
- Provides different error handling behavior than CLI operations
- Uses different configuration sources than CLI command processing
- Generates output formats that differ from CLI result formatting

## 5. Error Semantics Enforcement

### Error Rewriting Prohibitions
CI MUST reject any UI component that:
- Modifies CLI error messages or error code representations
- Translates technical error terms to user-friendly alternatives
- Adds explanatory text or context to CLI-generated error information
- Combines multiple CLI errors into summary or aggregate messages
- Filters or prioritizes error information based on content analysis

### Error Interpretation Prohibitions
CI MUST reject any UI component that:
- Assesses error severity or categorizes errors by type or impact
- Provides troubleshooting guidance or recovery suggestions for errors
- Infers error meaning or explains error implications to users
- Determines error recoverability or suggests retry strategies
- Creates error taxonomies or applies semantic error classification

### Retry Logic Prohibitions
CI MUST reject any UI component that:
- Implements automatic retry mechanisms for failed CLI operations
- Provides one-click retry functionality or convenience retry features
- Suggests optimal retry timing or retry parameter modifications
- Caches parameters for easy operation repetition after failures
- Implements progressive retry strategies or backoff algorithms

## 6. UI Replaceability Enforcement

### UI Removal Build Requirements
CI MUST ensure that:
- All non-UI build targets compile successfully with UI components removed
- CLI executables build and function identically without UI dependencies
- Core system tests pass completely with UI build targets disabled
- Adapter implementations remain buildable without Qt framework presence
- System functionality remains accessible through CLI with UI artifacts deleted

### Dependency Isolation Requirements
CI MUST reject any non-UI component that:
- Depends on UI-specific libraries or Qt framework components
- References UI-generated artifacts or UI-specific configuration files
- Requires UI presence for successful compilation or linking
- Uses UI-specific data formats or UI-generated schema definitions
- Contains conditional compilation that depends on UI availability

### Capability Independence Requirements
CI MUST ensure that:
- All CLI-accessible functionality remains available without UI components
- System configuration and operation proceed normally without UI presence
- Automated workflows and batch processing function without UI dependencies
- Monitoring and diagnostic capabilities operate independently of UI components
- All user-accessible workflows have complete CLI equivalents

## 7. Documentation Drift Detection

### Phase 15 Document Authority
CI MUST reject any UI implementation that:
- Contradicts architectural constraints defined in Phase 15 documentation
- Implements patterns explicitly prohibited in Phase 15 specifications
- Violates error handling rules defined in Phase 15.3-A documentation
- Bypasses adapter patterns specified in Phase 15.3-B documentation
- Fails UI deletion tests defined in Phase 15.4-A specifications

### Architectural Consistency Requirements
CI MUST ensure that:
- UI component implementations align with Phase 15 architectural principles
- Adapter implementations conform to Phase 15 stateless bridge requirements
- Error handling implementations match Phase 15 verbatim display rules
- UI lifecycle management follows Phase 15 replaceability constraints
- All UI functionality maintains Phase 15 CLI authority preservation

### Documentation Synchronization Requirements
CI MUST reject changes that:
- Modify UI behavior without corresponding Phase 15 document updates
- Introduce new UI capabilities not covered by Phase 15 specifications
- Change adapter interfaces without Phase 15 architectural review
- Alter error handling patterns without Phase 15 rule modification
- Implement UI features that contradict frozen Phase 15 contracts

## 8. Failure Conditions

### Hard CI Failure Triggers
CI SHALL fail immediately and unconditionally when:
- Any Qt UI component includes core engine headers directly
- Any adapter implementation contains Qt dependencies or stateful behavior
- Any UI component implements semantic processing or business logic
- Any non-UI component depends on UI artifacts or Qt framework presence
- Any implementation contradicts Phase 15 architectural documentation

### Build Termination Requirements
CI MUST terminate build processes when:
- Forbidden dependency patterns are detected in any UI component
- Adapter purity violations are found in CLI bridge implementations
- UI replaceability tests fail or UI removal breaks non-UI functionality
- Error handling implementations violate Phase 15 verbatim display rules
- Documentation drift is detected between implementation and Phase 15 specifications

### Enforcement Escalation Requirements
CI MUST treat as critical failures:
- Any attempt to bypass Phase 15 architectural constraints
- Any implementation that compromises CLI authority or adapter statelessness
- Any UI component that owns semantic functionality or execution control
- Any change that reduces system capability when UI components are removed
- Any modification that creates UI-specific behavior not available through CLI