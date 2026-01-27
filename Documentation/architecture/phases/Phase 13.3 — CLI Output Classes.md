# Phase 13.3 — CLI Output Classes

## Output Type Rules

- One output type ↔ one Phase 12 artifact
- Output types are opaque containers
- No CLI-only types
- No derived or merged views
- Names must be stable and versionable
- No transport assumptions (JSON/YAML/etc.)

## Phase 13 Output Type Registry

### Policy Outputs

```
PolicyDefinitionOutput
PolicyIndexOutput
PolicyHashOutput
```

### Interpretation Outputs

```
InterpretationResultOutput
InterpretationIndexOutput
InterpretationSummaryOutput
```

### Decision Outputs

```
DecisionRecordOutput
DecisionIndexOutput
DecisionTraceOutput
```

### Artifact Outputs

```
ArtifactRecordOutput
ArtifactIndexOutput
ArtifactDiffOutput
```

### Compatibility Outputs

```
CompatibilityMatrixOutput
CompatibilityCheckOutput
CompatibilityReportOutput
```

### Diagnostics Outputs

```
DiagnosticWarningOutput
DiagnosticErrorOutput
InvariantStatusOutput
```

### Schema Outputs

```
SchemaDescriptorOutput
SchemaIndexOutput
SchemaHashOutput
```

## Type Registry Summary

- **21 Output Types**: Perfect 1:1 correspondence with Phase 12 artifacts
- **7 Domain Groups**: Matching CLI command groups
- **Stable Names**: Versionable contract for CLI, Python bindings, CI automation
- **Opaque Containers**: No CLI-specific semantics or transformations

## Contract Guarantees

### CLI Output Contract
- Every CLI command produces exactly one output type
- Output type names are stable across CLI versions
- Output types contain complete Phase 12 artifact data

### Python Bindings Contract
- Output types become Python class names
- Direct mapping from CLI to Python API
- No impedance mismatch between interfaces

### CI/Automation Contract
- Output types define machine-consumable interface
- Stable names enable reliable automation
- No breaking changes to type names

## Explicitly Forbidden

- CLI-specific wrapper types
- Presentation-oriented names
- Transport-specific names (Json*, Yaml*)
- Aggregated or convenience outputs
- Renaming Phase 12 semantics