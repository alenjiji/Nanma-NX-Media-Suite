"""
Monitor command bindings - mirrors 'nx monitor' CLI group.
"""

from typing import Dict, Any, Literal
from ._cli import invoke_cli


def status(format: Literal["json", "text"] = "json") -> Dict[str, Any]:
    """
    Get monitor status information.
    
    Mirrors: nx monitor status --json|--text
    
    Args:
        format: Output format, either "json" or "text"
        
    Returns:
        Monitor status data as dict (when format="json")
        
    Raises:
        CLIError: If CLI command fails
        ValueError: If format is invalid
    """
    if format not in ("json", "text"):
        raise ValueError(f"Invalid format: {format}. Must be 'json' or 'text'")
    
    if format == "text":
        # For text format, we need to use raw invocation
        from ._cli import invoke_cli_raw
        return {"raw_output": invoke_cli_raw(["monitor", "status", "--text"])}
    
    # JSON format - use structured invocation
    return invoke_cli(["monitor", "status", "--json"])