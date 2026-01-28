"""
Phase 14B Python Bindings - Usage Examples

Demonstrates the CLI-mirrored pattern for all commands.
"""

import nx

# Example 1: Monitor status
try:
    status_data = nx.monitor.status(format="json")
    print("Monitor status:", status_data)
except nx._cli.CLIError as e:
    print(f"CLI failed: {e}")

# Example 2: Batch plan inspection (full implementation)
try:
    plan_data = nx.batch.inspect.plan(
        "batch_001",
        format="json",
        jobs_only=False,
        dag_only=False
    )
    print("Batch plan:", plan_data)
except nx._cli.CLIError as e:
    print(f"CLI failed: {e}")

# Example 3: Batch jobs listing (full implementation)
try:
    jobs_data = nx.batch.inspect.jobs(
        "batch_001",
        filter_type="convert",
        sort="id"
    )
    print("Batch jobs:", jobs_data)
except nx._cli.CLIError as e:
    print(f"CLI failed: {e}")

# Pattern for remaining commands (stubs to be implemented):
# nx.batch.inspect.status("batch_001", filter_state="failed")
# nx.batch.inspect.job("batch_001", "job_001", include_artifacts=True)
# nx.batch.inspect.policies("batch_001", job_id="job_001")
# nx.batch.inspect.artifacts("batch_001", artifact_type="report")
# nx.batch.inspect.artifact("batch_001", "artifact_001", format="json")

"""
Implementation Pattern for Remaining Commands:

1. Function signature mirrors CLI exactly:
   - Required positional args → function parameters
   - Optional flags → keyword arguments with defaults
   - Enum values → Literal types

2. Validation:
   - Check required parameters are not empty
   - Validate enum values against CLI contract
   - Raise ValueError for invalid combinations

3. CLI invocation:
   - Build args list: ["batch", "inspect", "command", ...positional]
   - Add flags only if different from CLI defaults
   - Call invoke_cli(args) and return result

4. Error handling:
   - Let CLIError propagate (preserves CLI exit codes)
   - Add ValueError for Python-level validation only

5. Return types:
   - Dict[str, Any] for JSON output
   - str for raw output
   - No custom classes or transformations
"""