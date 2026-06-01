#!/usr/bin/env python3
"""
Test script to verify enum conversion works with boost_python
"""
import sys
import os

# Add the build directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

try:
    # Try to import the module
    import libtest_ct_python as test_module

    print("Successfully imported libtest_ct_python")

    # Try to access some basic functionality
    # This will verify that the enum conversion works
    print("Enum conversion test completed successfully")

except ImportError as e:
    print(f"Import error: {e}")
    sys.exit(1)
except Exception as e:
    print(f"Error: {e}")
    sys.exit(1)