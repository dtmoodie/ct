import importlib
import os


if os.path.exists('tests/python/libtest_ct_python.so'):
    filename = 'tests/python/libtest_ct_python.so'
else:
    filename = 'tests/python/libtest_ct_pythond.so'

spec = importlib.util.spec_from_file_location('test_ct_python', filename)
mod = importlib.util.module_from_spec(spec)
