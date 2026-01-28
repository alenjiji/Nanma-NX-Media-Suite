"""
Nanma NX-MediaSuite Python Bindings (Phase 14B)
CLI-Mirrored Transport Layer

This package provides 1:1 Python bindings to the nx CLI.
All behavior is determined by the CLI - Python is transport only.
"""

__version__ = "14b.0.0"
__cli_version_required__ = "14a.0.0"

from . import monitor
from . import batch

__all__ = ["monitor", "batch"]