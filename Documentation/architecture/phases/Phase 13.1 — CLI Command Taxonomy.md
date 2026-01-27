# Phase 13.1 — CLI Command Taxonomy

## CLI Binary

```
nx-policy
```

## Top-Level Command Groups

```
nx-policy
├── policy
├── interpretation
├── decisions
├── artifacts
├── compatibility
├── diagnostics
└── schema
```

## Command Taxonomy

### policy
Raw Phase 11 policy material as consumed by Phase 12

```
nx-policy policy
├── list
├── show
└── hash
```

### interpretation
Resolved, read-only outputs of Phase 12

```
nx-policy interpretation
├── list
├── show
└── summary
```

### decisions
Explicit decision artifacts produced by interpretation

```
nx-policy decisions
├── list
├── show
└── trace
```

### artifacts
Materialized, auditable artifacts emitted by Phase 12

```
nx-policy artifacts
├── list
├── show
└── diff
```

### compatibility
Compatibility and constraint evaluation results

```
nx-policy compatibility
├── matrix
├── check
└── report
```

### diagnostics
Interpretation-time diagnostics (read-only)

```
nx-policy diagnostics
├── warnings
├── errors
└── invariants
```

### schema
Machine contracts for consumers (CLI / Python / CI)

```
nx-policy schema
├── list
├── show
└── hash
```

## Naming Constraints

- Commands reflect Phase 12 artifacts, not user intent
- Verbs are descriptive, not imperative
- No convenience aliases
- Flat where possible, nested where required for determinism
- Every command is read-only, deterministic, engine-agnostic

## Explicitly Forbidden

- `run`, `apply`, `execute`, `fix`, `validate`
- Any command implying mutation or action
- Any engine-specific naming
- Any UI-oriented grouping