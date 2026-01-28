# Phase 15.3 — Widget → Adapter Wiring Rules (Normative)

Status: FROZEN (Phase 15 Architectural Contract)
Scope: Qt UI Wiring Only
Authority: CLI Canonical Contract

# Widget → Adapter Wiring Rules

## 1. Wiring Responsibility Boundaries

### Widget Responsibilities
Widgets MUST:
- Collect user input parameters for CLI operations
- Display adapter results exactly as received
- Maintain presentation state only (scroll position, selection, visibility)
- Trigger adapter operations through defined interfaces
- Handle UI lifecycle events (show, hide, destroy)

Widgets MUST NOT:
- Interpret the meaning of adapter results
- Modify or transform CLI output data
- Make decisions about operation success or failure
- Store adapter results as authoritative application state
- Coordinate operations across multiple adapters

### Adapter Responsibilities
Adapters MUST:
- Execute CLI operations with provided parameters
- Return CLI results without modification or interpretation
- Terminate CLI subprocesses when explicitly requested
- Provide cancellation mechanisms for user-requested termination
- Isolate CLI execution from UI threading concerns

Adapters MUST NOT:
- Store results beyond single operation completion
- Make assumptions about widget display requirements
- Provide default values or fallback behaviors
- Retry failed operations automatically
- Share state between different widget instances

### Prohibited Responsibilities
Neither widgets nor adapters SHALL:
- Implement business logic or workflow orchestration
- Validate semantic correctness of parameters or results
- Provide user guidance or operation recommendations
- Cache results for reuse across operations
- Maintain persistent application state

## 2. Invocation Rules

### Widget Invocation Requirements
Widgets MUST:
- Pass complete parameter sets to adapters without modification
- Trigger operations only in response to explicit user actions
- Provide all required parameters as specified by CLI contracts
- Use adapter interfaces exactly as defined

Widgets MUST NOT:
- Add, remove, or modify parameters before adapter invocation
- Provide default values for missing parameters
- Combine multiple user actions into single adapter calls
- Invoke adapters based on inferred user intent

### Parameter Handling
Widgets SHALL:
- Collect parameters through explicit UI controls only
- Pass parameters to adapters in their original form
- Validate only syntactic parameter format (type, presence)
- Reject incomplete parameter sets without attempting completion

Widgets SHALL NOT:
- Enrich parameters with additional context or metadata
- Apply business rules to parameter validation
- Substitute alternative parameters for user-provided values
- Cache parameters for reuse in subsequent operations

## 3. Asynchrony & UI Responsiveness (Conceptual Only)

### UI Responsiveness Requirements
The wiring pattern MUST ensure:
- UI thread remains responsive during all adapter operations
- User interactions continue to function while operations execute
- UI controls provide immediate feedback for user actions
- Long-running operations do not block user interface updates

### Execution Isolation Rules
Adapters MUST:
- Execute CLI operations without blocking UI thread
- Deliver results back to UI thread through defined channels
- Provide mechanisms for user-requested operation cancellation
- Handle subprocess termination when requested

### Prohibited UI Execution Semantics
Widgets MUST NOT:
- Implement retry logic or automatic operation repetition
- Provide execution scheduling or queuing mechanisms
- Make decisions about operation timing or sequencing
- Implement timeout handling beyond basic UI responsiveness

## 4. Result Delivery Rules

### Result Reception Requirements
Widgets MUST:
- Accept adapter results exactly as provided
- Display all result data without filtering or selection
- Update UI upon result receipt
- Handle multiple results without automatic clearing

Widgets MUST NOT:
- Interpret result data to determine operation success
- Combine results from multiple operations
- Filter or prioritize result data based on content
- Store results beyond immediate display requirements

### Result Display Rules
Upon receiving adapter results, widgets SHALL:
- Present all provided data in unmodified form
- Use consistent formatting for similar data types
- Display error information exactly as received from CLI
- Maintain clear distinction between different result types

Widgets SHALL NOT:
- Summarize or aggregate result data
- Provide explanations or interpretations of results
- Suggest follow-up actions based on result content
- Assess result quality or completeness

## 5. Lifecycle & Ownership Rules

### Ownership Boundaries
Widgets SHALL:
- Create and own their associated adapter instances
- Ensure adapter lifecycle is bound to widget lifecycle
- Handle adapter disposal when widget is destroyed
- Maintain exclusive ownership of their adapter instances

### Creation and Disposal Rules
Widget-adapter relationships MUST:
- Be established during widget initialization
- Remain one-to-one throughout widget lifetime
- Be terminated cleanly during widget destruction
- Never involve shared adapter instances between widgets

### Cancellation Requirements
Widgets MUST:
- Provide user-accessible cancellation for operations
- Request adapter cancellation only when user initiates
- Handle cancellation without affecting other widget instances
- Ensure clean resource cleanup after user-requested cancellation

## 6. Error Propagation Wiring Rules

### Error Display Requirements
Widgets MUST:
- Display CLI error codes exactly as received
- Show CLI error messages without modification
- Present each error as discrete, unprocessed information
- Maintain error visibility until explicitly dismissed by user

Widgets MUST NOT:
- Translate or reword error messages for user-friendliness
- Categorize or group errors by type or severity
- Provide troubleshooting suggestions or retry guidance
- Attempt automatic error recovery or fallback operations

### Error State Management
Upon receiving adapter errors, widgets SHALL:
- Display error state and suspend dependent UI actions
- Mark any previously displayed results as stale and non-authoritative
- Provide clear visual distinction between error and success states
- Allow previously displayed results to remain visible if marked as stale

Widgets SHALL NOT:
- Attempt to determine error recoverability
- Provide automatic retry mechanisms
- Hide or minimize error information
- Continue normal operation flow after error receipt

## 7. State Containment Rules

### Permitted Temporary State
Widgets MAY hold:
- UI presentation state (scroll position, selection, expansion)
- Current operation parameters during collection
- Loading indicators and progress display state
- Error display state until user dismissal

### Prohibited State Persistence
Widgets MUST NOT:
- Store adapter results beyond current display cycle
- Maintain operation history or audit trails
- Cache results for reuse in subsequent operations
- Share state information between widget instances

### State Disposal Requirements
Widgets SHALL:
- Discard cached adapter results on navigation, refresh, or error
- Clear temporary state during widget destruction
- Reset to initial state after error conditions
- Ensure no state leakage between operation cycles

## 8. Replaceability Guarantee

### UI Deletion Impact
The wiring pattern MUST ensure:
- Complete UI removal has zero impact on CLI functionality
- All CLI operations remain accessible through direct invocation
- Adapter layer continues to function without UI components
- No business logic or system state depends on UI presence

### CLI Authority Preservation
Regardless of UI wiring:
- CLI remains the sole authoritative interface for all operations
- All functionality accessible through UI has direct CLI equivalents
- CLI behavior is unaffected by UI component presence or absence
- System state and operation results are CLI-controlled exclusively

### Adapter Independence
Adapters MUST:
- Function identically whether invoked by UI or other clients
- Maintain no dependencies on UI component lifecycle
- Provide consistent behavior across different invocation contexts
- Remain usable for automation, testing, and direct CLI access