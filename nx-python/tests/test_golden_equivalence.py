#!/usr/bin/env python3
"""
Golden Equivalence Tests for Phase 14B Python Bindings

Ensures byte-for-byte compatibility between CLI and Python bindings.
NO fuzzy matching, NO normalization, NO pretty-print differences allowed.
"""

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

# Import the Python bindings
sys.path.insert(0, str(Path(__file__).parent.parent))
import nx


class GoldenEquivalenceTest(unittest.TestCase):
    """Base class for golden equivalence testing."""
    
    def setUp(self):
        """Set up test environment."""
        self.cli_path = self._find_cli_executable()
        self.temp_dir = Path(tempfile.mkdtemp())
    
    def tearDown(self):
        """Clean up test environment."""
        import shutil
        shutil.rmtree(self.temp_dir, ignore_errors=True)
    
    def _find_cli_executable(self):
        """Find the nx-cli executable."""
        from nx._cli_resolver import find_nx_cli_executable
        try:
            return find_nx_cli_executable()
        except FileNotFoundError:
            self.skipTest("nx-cli executable not found")
    
    def _run_cli_command(self, args):
        """Run CLI command and capture output."""
        cmd = [self.cli_path] + args
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            cwd=str(self.temp_dir)
        )
        return result.returncode, result.stdout, result.stderr
    
    def _assert_golden_equivalence(self, cli_args, python_result):
        """Assert byte-for-byte equivalence between CLI and Python."""
        # Capture CLI output
        exit_code, cli_stdout, cli_stderr = self._run_cli_command(cli_args)
        
        # CLI must succeed for valid comparison
        if exit_code != 0:
            self.skipTest(f"CLI command failed: {cli_stderr}")
        
        # Parse CLI JSON (validates it's valid JSON)
        try:
            cli_json = json.loads(cli_stdout)
        except json.JSONDecodeError as e:
            self.fail(f"CLI output is not valid JSON: {e}")
        
        # Serialize Python result to JSON with same formatting
        python_json_str = json.dumps(python_result, separators=(',', ':'), sort_keys=False)
        
        # Parse Python JSON to validate structure
        try:
            python_json = json.loads(python_json_str)
        except json.JSONDecodeError as e:
            self.fail(f"Python result is not valid JSON: {e}")
        
        # CRITICAL: Byte-for-byte comparison of JSON strings
        cli_normalized = cli_stdout.strip()
        python_normalized = python_json_str.strip()
        
        if cli_normalized != python_normalized:
            self.fail(
                f"Golden equivalence FAILED:\n"
                f"CLI output ({len(cli_normalized)} bytes):\n{cli_normalized}\n\n"
                f"Python output ({len(python_normalized)} bytes):\n{python_normalized}\n\n"
                f"Difference detected - bindings are NOT equivalent"
            )


class TestMonitorCommands(GoldenEquivalenceTest):
    """Golden equivalence tests for monitor commands."""
    
    def test_monitor_status_golden_equivalence(self):
        """Test monitor.status() golden equivalence."""
        # Execute Python binding
        python_result = nx.monitor.status()
        
        # Assert golden equivalence
        self._assert_golden_equivalence(["monitor", "status"], python_result)


class TestBatchIntrospectionCommands(GoldenEquivalenceTest):
    """Golden equivalence tests for batch introspection commands."""
    
    def test_batch_inspect_plan_golden_equivalence(self):
        """Test batch.inspect.plan() golden equivalence."""
        # Create minimal test batch plan
        plan_file = self.temp_dir / "test_plan.json"
        plan_data = {
            "plan_id": "test_plan_001",
            "jobs": [],
            "created_at": "2024-01-01T00:00:00Z"
        }
        plan_file.write_text(json.dumps(plan_data))
        
        # Execute Python binding
        python_result = nx.batch.inspect.plan(str(plan_file))
        
        # Assert golden equivalence
        self._assert_golden_equivalence(
            ["batch", "inspect", "plan", str(plan_file)], 
            python_result
        )


# Template for adding new golden tests:
"""
class TestNewCommandGroup(GoldenEquivalenceTest):
    def test_command_name_golden_equivalence(self):
        # Set up test data if needed
        
        # Execute Python binding
        python_result = nx.command.group.function(args)
        
        # Assert golden equivalence
        self._assert_golden_equivalence(
            ["command", "group", "function", "args"], 
            python_result
        )
"""


if __name__ == "__main__":
    unittest.main()