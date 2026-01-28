"""
Core CLI invocation mechanism for Phase 14B bindings.
Implements exec-based subprocess strategy.
"""

import subprocess
import json
from typing import List, Dict, Any


class CLIError(Exception):
    """CLI command failed with non-zero exit code."""
    
    def __init__(self, exit_code: int, stderr_message: str):
        self.exit_code = exit_code
        self.stderr_message = stderr_message
        super().__init__(f"CLI failed (exit {exit_code}): {stderr_message}")


class CLIOutputError(Exception):
    """CLI output could not be parsed."""
    pass


def invoke_cli(args: List[str]) -> Dict[str, Any]:
    """
    Invoke nx CLI with given arguments and return parsed JSON output.
    
    Args:
        args: CLI arguments (excluding 'nx' binary name)
        
    Returns:
        Parsed JSON output as dict
        
    Raises:
        CLIError: CLI returned non-zero exit code
        CLIOutputError: CLI output was not valid JSON
    """
    # Force JSON output for all commands
    cmd = ['nx'] + args + ['--format', 'json']
    
    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=False  # Handle exit codes manually
    )
    
    if result.returncode != 0:
        raise CLIError(result.returncode, result.stderr.strip())
    
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError as e:
        raise CLIOutputError(f"Invalid JSON from CLI: {e}")


def invoke_cli_raw(args: List[str]) -> str:
    """
    Invoke nx CLI and return raw stdout (for non-JSON commands).
    
    Args:
        args: CLI arguments (excluding 'nx' binary name)
        
    Returns:
        Raw stdout string
        
    Raises:
        CLIError: CLI returned non-zero exit code
    """
    cmd = ['nx'] + args
    
    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=False
    )
    
    if result.returncode != 0:
        raise CLIError(result.returncode, result.stderr.strip())
    
    return result.stdout