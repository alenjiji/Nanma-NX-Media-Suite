# Phase 15.3-C — UI Error Display Rules

Status: FROZEN (Phase 15 Architectural Contract)
Scope: Qt UI Error Presentation
Authority: CLI Canonical Contract

# UI Error Display Rules

## 1. Core Error Display Principle

Qt widgets SHALL display CLI errors exactly as received without modification, interpretation, or enhancement. The CLI is the sole authority for error content, meaning, and significance.

## 2. Verbatim Error Preservation

### Error Code Display
Widgets MUST:
- Display CLI error codes exactly as returned
- Preserve original error code format, casing, and punctuation
- Show complete error code strings without truncation or abbreviation
- Maintain error code ordering as provided by CLI

Widgets MUST NOT:
- Translate error codes to alternative representations
- Map error codes to UI-friendly identifiers
- Group or categorize error codes by type or domain
- Modify error code formatting for display consistency

### Error Message Display
Widgets MUST:
- Display CLI error messages without any textual modification
- Preserve original message formatting, whitespace, and line breaks
- Show complete error messages without truncation or ellipsis
- Maintain message ordering exactly as provided by CLI

Widgets MUST NOT:
- Reword error messages for user-friendliness or clarity
- Correct grammar, spelling, or formatting in error messages
- Translate technical terms to plain language equivalents
- Add explanatory text or context to error messages

### Formatting Preservation
Widgets SHALL:
- Preserve all CLI-provided formatting including spacing and punctuation
- Display multi-line error messages with original line structure
- Maintain character encoding and special characters as provided
- Show error data in monospace or fixed-width fonts when appropriate

### Completeness Requirements
Widgets MUST:
- Display all error information provided by CLI without omission
- Show every error in a multi-error response
- Present error details, context, and metadata exactly as received
- Ensure no error information is hidden, collapsed, or summarized

## 3. Prohibited Error Transformations

### Summarization Prohibitions
Widgets MUST NOT:
- Create summary statements for multiple errors
- Generate overview text describing error patterns or themes
- Combine related errors into single display items
- Provide count-based summaries or statistical aggregations

### Rewording Prohibitions
Widgets MUST NOT:
- Substitute alternative language for CLI error text
- Simplify technical language for general audiences
- Add explanatory phrases or clarifying statements
- Modify error tone or urgency indicators

### Classification Prohibitions
Widgets MUST NOT:
- Assign severity levels to errors beyond CLI-provided information
- Group errors by type, category, or functional area
- Apply color coding or visual indicators based on error interpretation
- Create taxonomies or hierarchies of error types

### Severity Labeling Prohibitions
Widgets MUST NOT:
- Add severity indicators such as "Critical", "Warning", or "Info"
- Prioritize error display based on perceived importance
- Apply visual emphasis based on error content assessment
- Create urgency indicators beyond CLI-provided formatting

### Interpretation Prohibitions
Widgets MUST NOT:
- Explain what errors mean or their likely causes
- Suggest relationships between different errors
- Infer system state or health from error patterns
- Provide context about error implications or consequences

## 4. Retry & Recovery Prohibitions

### Retry Suggestion Prohibitions
Widgets MUST NOT:
- Suggest specific retry actions or procedures
- Indicate whether errors are recoverable or permanent
- Provide guidance on parameter modification or correction
- Offer alternative approaches or workarounds

### Recovery Guidance Prohibitions
Widgets MUST NOT:
- Recommend troubleshooting steps or diagnostic procedures
- Suggest system configuration changes or fixes
- Provide links to documentation or help resources
- Indicate expected resolution timeframes or effort

### Automation Prohibitions
Widgets MUST NOT:
- Implement automatic retry mechanisms for failed operations
- Provide one-click retry buttons or similar convenience features
- Cache parameters for easy operation repetition
- Suggest or enable batch retry operations

### Inference Prohibitions
Widgets MUST NOT:
- Assess error recoverability or permanence
- Determine whether errors indicate user mistakes or system issues
- Evaluate error frequency or patterns for guidance
- Make assumptions about error resolution requirements

## 5. Error Visibility & Lifecycle

### Error Appearance Rules
Widgets MUST:
- Display errors immediately upon receipt from CLI adapter
- Show errors in dedicated, clearly identified error display areas
- Ensure error information is visible without user interaction
- Maintain error visibility until explicitly cleared by user action

### Error Persistence Rules
Widgets SHALL:
- Keep errors visible until user explicitly dismisses them
- Maintain error display across UI navigation within the same widget
- Preserve error information during widget resize or layout changes
- Show errors consistently regardless of other UI state changes

### Error Clearing Rules
Widgets MAY clear errors only when:
- User explicitly dismisses error display through direct action
- New CLI operation is initiated that produces new results
- Widget is destroyed or permanently closed
- User explicitly requests error clearing through dedicated controls

### Auto-Dismissal Prohibitions
Widgets MUST NOT:
- Automatically hide errors after time delays or timeouts
- Clear errors based on user activity in other UI areas
- Dismiss errors when successful operations complete
- Remove error display due to UI state changes or navigation

## 6. Relationship to Successful Results

### Error-Result Interaction Rules
When errors occur, widgets SHALL:
- Display error state prominently and suspend dependent UI actions
- Mark any previously displayed results as stale and non-authoritative
- Maintain clear visual distinction between error and success states
- Allow previously displayed results to remain visible if clearly marked as stale

### Stale Result Marking
Widgets MUST:
- Clearly indicate when displayed results are no longer current due to errors
- Use visual indicators to distinguish stale results from authoritative data
- Prevent user actions that depend on stale result data
- Maintain stale result visibility without implying continued validity

### UI Judgment Prohibitions
Widgets MUST NOT:
- Assess whether errors invalidate previous results
- Determine which results remain valid after error conditions
- Make decisions about result reliability or trustworthiness
- Evaluate error impact on previously successful operations

### Authority Preservation
Widgets SHALL:
- Treat all CLI output as equally authoritative regardless of success or failure
- Display error and success information with equivalent fidelity
- Avoid implying that successful results are more reliable than error information
- Maintain CLI authority over all result interpretation and significance

## 7. Replaceability & CLI Authority

### UI Deletion Impact
These error display rules ensure:
- Complete UI removal has zero effect on CLI error reporting
- All error information remains accessible through direct CLI invocation
- Error handling behavior is identical with or without UI presence
- No error processing logic depends on UI component existence

### CLI Error Authority
Regardless of UI presence:
- CLI remains the sole source of error definitions and meanings
- Error significance and interpretation authority belongs exclusively to CLI
- UI provides no additional error processing or enhancement capabilities
- All error-related functionality has direct CLI equivalents

### Python/CLI Parity
Error display rules guarantee:
- Python bindings receive identical error information as UI
- Automated systems have equivalent error access without UI dependencies
- Error processing workflows function identically across all interfaces
- No UI-specific error handling or interpretation exists

## 8. Explicit Non-Capabilities (Enforcement Clause)

Qt widgets are deliberately incapable of:

* interpreting error meaning or significance
* assessing error severity or priority
* determining error recoverability or permanence
* suggesting error resolution approaches
* providing error context or explanation
* categorizing or classifying errors
* summarizing or aggregating error information
* making judgments about error impact or consequences

Widgets display CLI error output only.
All error interpretation, assessment, and response decisions belong exclusively to the CLI and the user.