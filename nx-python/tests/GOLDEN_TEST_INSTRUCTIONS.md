# Golden Equivalence Test Instructions

## Overview

Golden equivalence tests ensure byte-for-byte compatibility between CLI commands and Python bindings. These tests are **mandatory** for Phase 14B and enforce strict determinism requirements.

## Test Structure

### Base Class: `GoldenEquivalenceTest`

All golden tests inherit from `GoldenEquivalenceTest` which provides:
- CLI executable discovery
- Temporary directory setup/cleanup
- CLI command execution
- Byte-for-byte JSON comparison

### Core Method: `_assert_golden_equivalence(cli_args, python_result)`

This method:
1. Executes CLI command with given arguments
2. Captures CLI stdout as JSON
3. Serializes Python result to JSON
4. Performs **exact** byte-for-byte comparison
5. Fails on ANY divergence

## Adding New Golden Tests

### Step 1: Create Test Class

```python
class TestYourCommandGroup(GoldenEquivalenceTest):
    """Golden equivalence tests for your command group."""
```

### Step 2: Implement Test Method

```python
def test_your_command_golden_equivalence(self):
    """Test your.command() golden equivalence."""
    
    # Set up test data if needed
    test_file = self.temp_dir / "test_data.json"
    test_file.write_text('{"test": "data"}')
    
    # Execute Python binding
    python_result = nx.your.command(str(test_file))
    
    # Assert golden equivalence
    self._assert_golden_equivalence(
        ["your", "command", str(test_file)], 
        python_result
    )
```

### Step 3: Add to Test Runner

The test runner automatically discovers tests matching `test_golden_*.py` pattern.

## Command Mapping

For each CLI command, create corresponding golden test:

| CLI Command | Python Binding | Test Method |
|-------------|----------------|-------------|
| `monitor status` | `nx.monitor.status()` | `test_monitor_status_golden_equivalence` |
| `batch inspect plan <file>` | `nx.batch.inspect.plan(file)` | `test_batch_inspect_plan_golden_equivalence` |
| `batch inspect jobs <file>` | `nx.batch.inspect.jobs(file)` | `test_batch_inspect_jobs_golden_equivalence` |
| `batch inspect status <file>` | `nx.batch.inspect.status(file)` | `test_batch_inspect_status_golden_equivalence` |
| `batch inspect job <file> <id>` | `nx.batch.inspect.job(file, id)` | `test_batch_inspect_job_golden_equivalence` |
| `batch inspect policies <file>` | `nx.batch.inspect.policies(file)` | `test_batch_inspect_policies_golden_equivalence` |
| `batch inspect artifacts <file>` | `nx.batch.inspect.artifacts(file)` | `test_batch_inspect_artifacts_golden_equivalence` |
| `batch inspect artifact <file> <path>` | `nx.batch.inspect.artifact(file, path)` | `test_batch_inspect_artifact_golden_equivalence` |

## Critical Requirements

### NO Fuzzy Matching
- Exact byte-for-byte comparison required
- Field ordering must be preserved
- No normalization allowed

### NO Pretty-Print Differences
- JSON formatting must be identical
- Whitespace must match exactly
- No reformatting allowed

### Fail on ANY Divergence
- Single character difference = test failure
- No tolerance for "close enough"
- Determinism is mandatory

## Running Tests

```bash
# Run all golden tests
cd nx-python/tests
python run_golden_tests.py

# Run specific test class
python run_golden_tests.py TestMonitorCommands

# Verbose output
python run_golden_tests.py -v

# Run single test method
python -m unittest test_golden_equivalence.TestMonitorCommands.test_monitor_status_golden_equivalence
```

## Test Data Setup

For commands requiring input files:

```python
def test_command_with_file_golden_equivalence(self):
    # Create test file in temp directory
    test_file = self.temp_dir / "input.json"
    test_data = {"required": "structure"}
    test_file.write_text(json.dumps(test_data))
    
    # Execute and test
    python_result = nx.command.with_file(str(test_file))
    self._assert_golden_equivalence(
        ["command", "with-file", str(test_file)], 
        python_result
    )
```

## Error Handling

Tests automatically handle:
- CLI executable not found (skip test)
- CLI command failure (skip test)
- Invalid JSON output (fail test)
- Byte-for-byte mismatch (fail test with diff)

## Integration with CI/CD

Golden tests should be run:
- Before every commit
- In CI pipeline
- Before releases
- After CLI changes

Failure of any golden test **blocks** the release.

## Debugging Failed Tests

When a golden test fails:

1. Check the exact byte difference in test output
2. Verify CLI command works independently
3. Check Python binding implementation
4. Ensure JSON serialization matches CLI format
5. Verify field ordering preservation

## Adding Commands Checklist

- [ ] Implement Python binding function
- [ ] Create golden equivalence test
- [ ] Add test data setup if needed
- [ ] Run test to verify equivalence
- [ ] Add to CI pipeline
- [ ] Document in command mapping table