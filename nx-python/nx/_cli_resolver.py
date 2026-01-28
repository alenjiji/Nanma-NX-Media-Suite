"""
Shared CLI executable resolver for nx-python bindings.

Provides deterministic nx-cli executable discovery without PATH dependency.
Used by both golden tests and runtime CLI invocation.
"""

from pathlib import Path


def find_nx_cli_executable():
    """
    Find the nx-cli executable using deterministic path resolution.
    
    Returns:
        str: Absolute path to nx-cli executable
        
    Raises:
        FileNotFoundError: If nx-cli executable cannot be located
    """
    # Resolve to repository root: nx-python/nx/_cli_resolver.py -> nanma_nx_media_suite/
    project_root = Path(__file__).resolve().parents[2]
    
    candidates = [
        # Single-config generators (Unix, Ninja)
        project_root / "build" / "nx-cli" / "nx-cli",
        project_root / "build" / "nx-cli" / "nx-cli.exe",
        
        # MSVC multi-config (Release / Debug)
        project_root / "build" / "nx-cli" / "Release" / "nx-cli.exe",
        project_root / "build" / "nx-cli" / "Debug" / "nx-cli.exe",
        
        # Alternate layout (nx-cli subproject)
        project_root / "nx-cli" / "build" / "nx-cli",
        project_root / "nx-cli" / "build" / "nx-cli.exe",
        project_root / "nx-cli" / "build" / "Release" / "nx-cli.exe",
        project_root / "nx-cli" / "build" / "Debug" / "nx-cli.exe",
    ]
    
    for candidate in candidates:
        if candidate.exists():
            return str(candidate.resolve())
    
    raise FileNotFoundError(
        f"nx-cli executable not found. Searched paths:\n" +
        "\n".join(f"  {candidate}" for candidate in candidates)
    )