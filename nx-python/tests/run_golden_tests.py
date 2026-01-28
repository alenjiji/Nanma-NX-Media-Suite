#!/usr/bin/env python3
"""
Test Runner for Golden Equivalence Tests

Usage:
    python run_golden_tests.py                    # Run all tests
    python run_golden_tests.py TestMonitorCommands # Run specific test class
    python run_golden_tests.py -v                 # Verbose output
"""

import sys
import unittest
from pathlib import Path

# Add nx-python to path
sys.path.insert(0, str(Path(__file__).parent.parent))

# Import test modules
from test_golden_equivalence import *

if __name__ == "__main__":
    # Configure test runner
    loader = unittest.TestLoader()
    
    if len(sys.argv) > 1 and not sys.argv[1].startswith('-'):
        # Run specific test class
        suite = loader.loadTestsFromName(sys.argv[1], module=sys.modules[__name__])
    else:
        # Run all tests
        suite = loader.discover('.', pattern='test_golden_*.py')
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2 if '-v' in sys.argv else 1)
    result = runner.run(suite)
    
    # Exit with error code if tests failed
    sys.exit(0 if result.wasSuccessful() else 1)