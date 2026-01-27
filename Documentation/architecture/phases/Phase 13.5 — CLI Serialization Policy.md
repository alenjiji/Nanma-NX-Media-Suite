# Phase 13.5 — CLI Serialization Policy

## Serialization Core Principle

> **Serialization is a transparent encoding of Phase 12 output types.**
> It must never change meaning, structure, ordering, or precision.

## Serialization Guarantees

### 1. Determinism
- Identical Phase 12 artifact → byte-identical serialized output
- No timestamps
- No environment-dependent fields
- No locale influence
- Stable ordering for all collections

### 2. Losslessness
- Every field present in Phase 12 output type is serialized
- No truncation
- No rounding
- No omission of "empty" fields unless Phase 12 omits them

### 3. Structural Fidelity
- Serialization mirrors Phase 12 data model
- No CLI-specific reshaping
- No flattening
- No renaming
- No aggregation

### 4. Explicitness
- No inferred fields
- No default values injected
- Absence must be explicit (null / empty / missing per Phase 12 contract)

## Allowed Serialization Formats

Phase 13 may support multiple formats, but they are equivalent views.

### Allowed
- JSON (canonical, machine-first)
- YAML (human-readable, secondary)
- Plain text (only for hashes / single scalars)

### Forbidden
- Binary-only formats
- Pretty-print-only variants
- Format-specific semantics

## Canonical Ordering Rules

- **Object keys**: lexicographically sorted
- **Arrays**: preserve Phase 12 order exactly
- **Maps**: sorted by key unless Phase 12 specifies ordering

## Numeric & Scalar Rules

- **Integers**: serialized as integers
- **Floats**: serialized with full precision (no rounding)
- **Booleans**: explicit true / false
- **Enums**: serialized as stable symbolic names, not ordinals
- **Hashes**: lowercase hex, fixed width

## Error Serialization

- Errors are serialized as error artifacts
- CLI never formats or summarizes errors
- Exit code communicates category; output contains full detail

## Explicitly Forbidden

- Pretty-print decisions affecting semantics
- Human-friendly abbreviations
- Dropping zero / false / empty values
- Re-labeling fields
- Collapsing nested structures
- CLI-only metadata (timestamps, version banners)

## Contract Validation

### Determinism Test
```
Same Phase 12 artifact → Same serialized bytes
```

### Losslessness Test
```
Phase 12 artifact → Serialize → Deserialize → Identical artifact
```

### Format Equivalence Test
```
JSON output ≡ YAML output (modulo syntax)
```

### Ordering Stability Test
```
Multiple serializations → Identical key ordering
```