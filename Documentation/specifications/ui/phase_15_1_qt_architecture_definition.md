# Phase 15.1 — Qt UI Architecture & Invariants

**Status:** Architectural Definition Complete
**Implementation Status:** Not Started (Architecture-First Approach)
**Next Phase:** Phase 15.2 (UI Adapter Implementation)

---

## Architectural Position Statement

### Qt UI as Final Consumer

Qt UI occupies the terminal position in the system architecture, consuming only through established contracts:

```
Policy (Phase 11) → Interpretation (Phase 12) → CLI (Phase 13-14A) → Python (Phase 14B) → Qt UI (Phase 15)
```

**Fundamental Truth:** Qt UI is downstream-only. It cannot be a peer to CLI or Python bindings.

### Canonical Contract Authority

**The CLI is the only behavioral contract.** Qt UI must:
- Invoke exact CLI commands via subprocess
- Consume exact CLI outputs without interpretation
- Parse exact CLI DTOs and serialization formats
- Obey exact CLI error semantics and exit codes

**Explicitly Forbidden:**
- Direct calls to Phase 12 interpretation engine
- Direct calls to core engines or batch systems
- UI-only flags, shortcuts, or default behaviors
- "Friendly" behavior not exposed by CLI

---

## Dependency Architecture Rules

### Allowed Dependency Flow
```
Qt Widgets/UI Components
        ↓
Qt UI Adapters (Stateless)
        ↓
CLI Invocation Layer
        ↓
nx-cli Executable (Frozen Contract)
```

### Forbidden Dependencies (Hard Ban)
- Qt → Phase 12 interpretation engine
- Qt → Core engines (nx-core)
- Qt → Batch engine directly
- Qt → Policy objects directly
- Qt → Python bindings (Python mirrors CLI, not UI)

**No exceptions permitted under any circumstances.**

---

## Qt UI Responsibilities (Exhaustive List)

### 1. Viewer Responsibilities
- Display CLI output without semantic modification
- Visualize batch state as reported by CLI
- Render timelines, logs, and reports from CLI data
- Show policy interpretations as text/data only

### 2. Dispatcher Responsibilities  
- Construct CLI invocations from user input
- Pass user-selected arguments to CLI commands
- Trigger batch runs, status queries, and exports via CLI

### 3. Presenter Responsibilities
- Sorting and filtering of CLI output data
- Pagination of large CLI result sets
- Visualization (tables, trees, graphs) of CLI data
- UI state management (window geometry, column widths)

**Critical Constraint:** Presentation transforms are allowed. Semantic transforms are forbidden.

---

## Forbidden UI Responsibilities (Absolute Prohibitions)

Qt UI must never:
- Interpret policy meaning or add policy logic
- Infer defaults not provided by CLI
- Modify parameters implicitly or "helpfully"
- Hide required CLI flags from user
- Reorder execution semantics
- Retry jobs or implement retry logic
- Merge batch results beyond visualization
- Aggregate state beyond presentation layer
- Cache authoritative results (only UI state)
- "Fix" user input before CLI invocation
- Override or "improve" CLI error messages

### Replaceability Test
**If removing the UI changes system behavior, the UI architecture is broken.**

---

## State Ownership Rules

### Allowed UI State (Ephemeral Only)
- Window geometry and layout preferences
- Column widths and sort order
- Expanded/collapsed tree nodes
- Last-used CLI arguments (as history, not defaults)
- UI presentation preferences

### Forbidden UI State (Authoritative)
- Job execution state
- Batch ownership or lifecycle state
- Policy interpretation state
- Retry counters or execution metadata
- Any derived business logic state

**All authoritative state lives outside Qt in CLI-accessible systems.**

---

## Mandatory Adapter Pattern

### UI Adapter Characteristics
Qt must communicate only through UI adapters with these properties:
- **Stateless:** No persistent business state
- **Thin:** Minimal logic, maximum CLI delegation
- **Isolated:** One adapter per CLI command group
- **Testable:** Unit-testable without Qt dependencies
- **Mockable:** CLI invocation can be mocked for testing

### Adapter Interface Pattern
```
Input: User parameters → CLI argument construction
Process: CLI invocation via subprocess
Output: CLI response → DTO parsing → UI data structures
Error: CLI error codes → UI error display (verbatim)
```

**Adapters are the only bridge. Qt widgets must not "know" the system.**

---

## Error Handling Invariants

### CLI Error Preservation (Sacred)
Qt UI must preserve CLI errors exactly:
- Display error codes verbatim without translation
- Display error messages verbatim without "friendly" rewrites
- Preserve exit code semantics for user understanding
- Maintain deterministic error behavior

### Forbidden Error Handling
- Rewriting error messages for "user friendliness"
- Collapsing multiple errors into summaries
- Automatic retry of failed operations
- Guessing recovery paths or suggesting fixes
- Hiding CLI error details from user

---

## Batch & Monitoring Constraints

### Allowed Monitoring Behavior
- Subscribe to batch status via CLI polling
- Poll deterministically using CLI commands
- Visualize timelines and metrics from CLI data
- Display real-time status updates from CLI

### Forbidden Execution Influence
- Drive job scheduling or execution order
- Pause, resume, or cancel jobs directly
- Throttle resources or influence performance
- Intervene in failure handling or retry logic
- Modify batch execution parameters

**Monitoring remains observe-only, exactly as defined in Phase 13-14.**

---

## Enforcement Mechanisms

### Design-Time Requirements (Phase 15.1)
Before any UI implementation begins:
- Define explicit UI → CLI adapter interfaces
- Create compile-time Qt-free adapter tests
- Establish CI rule: UI build must not link core symbols
- Establish CI rule: No Phase 12 headers in UI targets
- Establish CI rule: CLI output golden tests for UI parsers

### Runtime Enforcement (Phase 15.2+)
During and after UI implementation:
- Automated testing of adapter isolation
- CLI contract compliance verification
- UI deletion tests (system remains complete)
- Golden equivalence tests for UI → CLI → Python consistency

---

## Phase 15 Success Criteria

Phase 15 will be considered architecturally successful only if:

### Provable Thinness
- UI demonstrably adds zero business semantics
- All business logic remains in CLI-accessible systems
- UI deletion causes no behavioral change to system

### Contract Compliance
- CLI remains canonical interface
- Python binding parity remains intact
- No UI-specific behavior exists

### Architectural Integrity
- Dependency rules enforced at compile time
- Adapter pattern successfully isolates Qt from core
- Error handling preserves CLI authority

---

## Mental Model for Phase 15 Proceedings

### Phase 15.1: Architecture Definition ✅ COMPLETE
- Establish UI invariants and constraints
- Define adapter pattern requirements
- Create enforcement mechanisms
- Document forbidden dependencies

### Phase 15.2: UI Adapter Implementation (Next)
- Implement stateless CLI adapters
- Create adapter unit tests (Qt-free)
- Establish CLI invocation patterns
- Verify adapter isolation

### Phase 15.3: Qt Widget Implementation
- Create Qt UI components using adapters only
- Implement presentation layer (sorting, filtering, visualization)
- Add UI state management (ephemeral only)
- Verify no direct core dependencies

### Phase 15.4: Integration & Testing
- End-to-end UI → CLI → Core flow testing
- Golden equivalence verification (UI → CLI → Python)
- UI deletion tests (system completeness)
- Performance and usability validation

### Phase 15.5: Documentation & Closure
- Complete UI architecture documentation
- Create maintenance guidelines
- Establish UI evolution constraints
- Final architectural compliance verification

---

## Architectural Confidence Statement

**Phase 15.1 establishes the architectural foundation for safe Qt UI development.**

The constraints defined here ensure:
- CLI authority is preserved
- Core system integrity is maintained  
- UI remains replaceable and non-essential
- Future UI evolution cannot violate system architecture

**Proceeding to Phase 15.2 (UI Adapter Implementation) is architecturally safe.**