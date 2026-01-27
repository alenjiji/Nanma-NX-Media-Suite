# Phase 13.2 — Phase 12 → CLI Mapping Table

## Mapping Rules

- Every CLI command maps to exactly one Phase 12 artifact class
- No command may combine multiple artifacts
- No artifact may be partially exposed
- CLI output = serialized Phase 12 artifact as-is
- Absence of data = empty result, not an error (unless Phase 12 reports error)

## Phase 12 → Phase 13 Mapping Table

| Phase 12 Artifact Class | Description | CLI Command |
|------------------------|-------------|-------------|
| PolicyDefinition | Raw, immutable Phase 11 policy objects | `nx-policy policy show` |
| PolicySetIndex | Index of loaded policies | `nx-policy policy list` |
| PolicyHash | Deterministic content hash | `nx-policy policy hash` |
| InterpretationResult | Fully resolved interpretation state | `nx-policy interpretation show` |
| InterpretationIndex | Index of interpretation runs | `nx-policy interpretation list` |
| InterpretationSummary | Aggregated, non-derived summary | `nx-policy interpretation summary` |
| DecisionRecord | Individual explicit decision | `nx-policy decisions show` |
| DecisionIndex | Ordered decision list | `nx-policy decisions list` |
| DecisionTrace | Full decision trace graph | `nx-policy decisions trace` |
| ArtifactRecord | Materialized output artifact | `nx-policy artifacts show` |
| ArtifactIndex | Artifact inventory | `nx-policy artifacts list` |
| ArtifactDiff | Structural diff between artifacts | `nx-policy artifacts diff` |
| CompatibilityMatrix | Compatibility resolution matrix | `nx-policy compatibility matrix` |
| CompatibilityCheck | Point-in-time compatibility result | `nx-policy compatibility check` |
| CompatibilityReport | Full compatibility report | `nx-policy compatibility report` |
| DiagnosticWarning | Interpretation warnings | `nx-policy diagnostics warnings` |
| DiagnosticError | Interpretation errors | `nx-policy diagnostics errors` |
| InvariantStatus | Deterministic invariant checks | `nx-policy diagnostics invariants` |
| SchemaDescriptor | Machine-consumable schema | `nx-policy schema show` |
| SchemaIndex | Available schemas | `nx-policy schema list` |
| SchemaHash | Schema content hash | `nx-policy schema hash` |

## Mapping Constraints

### 1:1 Correspondence
- 21 Phase 12 artifact classes
- 21 CLI commands
- Perfect bijection between artifacts and commands

### Lossless Projection
- CLI output contains complete Phase 12 artifact
- No data transformation or reformatting
- No filtering or aggregation beyond Phase 12

### Read-Only Guarantee
- CLI never modifies Phase 12 artifacts
- CLI never triggers Phase 12 operations
- CLI only serializes existing artifacts

## Explicitly Forbidden Mappings

- One command → multiple artifacts
- Derived or reformatted outputs
- CLI-only summaries
- Filtering, sorting, grouping not present in Phase 12
- "Helpful" projections