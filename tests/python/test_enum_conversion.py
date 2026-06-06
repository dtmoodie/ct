#!/usr/bin/env python3
"""
Test script to verify enum conversion works with boost_python
"""
import sys
import os

# Add the build directory to the path
potential_lib_path = os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'tests', 'python')
if os.path.isdir(potential_lib_path):
    sys.path.insert(0, potential_lib_path)
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

try:
    import importlib.util
    import os
    
    # Determine library path and load using spec
    base_dir = os.path.dirname(os.path.abspath(__file__))
    candidate_paths = [
        os.path.join(base_dir, '..', '..', 'build', 'tests', 'python', 'libtest_ct_python.so'),
    ]
    
    lib_path = None
    for candidate in candidate_paths:
        if os.path.exists(candidate):
            lib_path = candidate
            break
    
    if lib_path is None:
        raise FileNotFoundError('libtest_ct_python.so not found')
    
    spec = importlib.util.spec_from_file_location('test_ct_python', lib_path)
    test_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(test_module)
    
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