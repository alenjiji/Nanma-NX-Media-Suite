# CI/CD Pipeline - Phase 14B Gating

## Overview

The CI pipeline enforces Phase 14B contract compliance through automated golden equivalence testing. Any divergence between CLI and Python bindings blocks merges.

## Required Checks for Merge

### Phase 14B Golden Equivalence Gate
- **Workflow:** `.github/workflows/phase-14b-gate.yml`
- **Trigger:** All pushes and pull requests to `main`
- **Purpose:** Enforce CLI ↔ Python binding equivalence
- **Status:** **REQUIRED** - merge blocked on failure

#### Test Flow:
1. Build nx-cli executable on Linux
2. Run golden equivalence tests: `nx-python/tests/run_golden_tests.py -v`
3. Validate byte-for-byte JSON output equivalence
4. Pass/fail based on test results

#### Test Outcomes:
- **PASS:** All tests pass or are legitimately skipped → merge allowed
- **FAIL:** Any golden test fails → merge blocked
- **SKIP:** Tests skipped due to missing CLI state → merge allowed (logged)

### Linux Build
- **Workflow:** `.github/workflows/linux-build.yml`  
- **Trigger:** All pushes and pull requests to `main`
- **Purpose:** Verify project builds successfully on Linux
- **Status:** Informational (not gating)

### Versioning
- **Workflow:** `.github/workflows/versioning.yml`
- **Trigger:** Pushes to `main` (tag creation)
- **Purpose:** Semantic versioning and release tagging
- **Status:** Post-merge automation

## Phase 14B Contract Enforcement

### What is Tested:
- CLI command: `nx monitor status --json`
- Python binding: `nx.monitor.status()`
- CLI command: `nx batch inspect plan <file> --format json`
- Python binding: `nx.batch.inspect.plan(file)`
- Additional commands as implemented

### Equivalence Requirements:
- **Byte-for-byte JSON output** must be identical
- **Field ordering** must be preserved
- **No normalization** or pretty-printing differences allowed
- **Exit codes** must match between CLI and Python

### Failure Scenarios:
- CLI output differs from Python output by any character
- Python binding adds/removes fields not in CLI
- Field ordering changes between CLI and Python
- CLI succeeds but Python fails (or vice versa)

## Branch Protection Rules

To enable merge gating, configure branch protection on `main`:

1. **Require status checks to pass before merging**
2. **Required status checks:**
   - `Phase 14B Golden Equivalence Gate / Python CLI Golden Equivalence`
3. **Require branches to be up to date before merging**
4. **Restrict pushes that create files**

## Debugging Failed Gates

### Golden Equivalence Test Failure:
1. Check test output for exact byte differences
2. Run tests locally: `cd nx-python/tests && python3 run_golden_tests.py -v`
3. Verify CLI works independently: `build/nx-cli/nx-cli monitor status --json`
4. Update Python bindings to match CLI exactly
5. Re-run tests to verify fix

### CLI Build Failure:
1. Check CMake configuration errors
2. Verify all source files compile
3. Check for missing dependencies
4. Ensure nx-cli target builds successfully

## Local Testing

Before pushing changes:

```bash
# Build nx-cli
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target nx-cli

# Run golden tests
cd nx-python/tests
PYTHONPATH=.. python3 run_golden_tests.py -v
```

## Maintenance

### Adding New CLI Commands:
1. Implement CLI command
2. Implement Python binding following mirror pattern
3. Add golden equivalence test
4. Verify test passes locally
5. Push - CI will validate equivalence automatically

### CLI Interface Changes:
1. Update CLI implementation
2. Update corresponding Python binding
3. Update golden test if needed
4. Verify equivalence locally
5. Push - CI will catch any missed updates

## Emergency Bypass

In exceptional circumstances, the Phase 14B gate can be bypassed by:
1. Temporarily removing the required status check
2. Merging the critical fix
3. Immediately re-enabling the required status check
4. Filing follow-up issue to restore equivalence

**Note:** Bypass should only be used for critical security fixes or system outages.