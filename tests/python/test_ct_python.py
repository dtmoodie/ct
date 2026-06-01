import importlib
import os
import sys

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to find the library file
lib_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'libtest_ct_python.so')
if os.path.exists(lib_path):
    filename = lib_path
else:
    # Fallback to just the filename
    filename = 'libtest_ct_python.so'

spec = importlib.util.spec_from_file_location('test_ct_python', filename)
mod = importlib.util.module_from_spec(spec)
