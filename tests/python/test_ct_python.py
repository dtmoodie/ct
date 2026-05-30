import importlib
import os

if os.path.exists('libtest_ct_python.so'):
    filename = 'libtest_ct_python.so'
else:
    filename = 'libtest_ct_pythond.so'
    
spec = importlib.util.spec_from_file_location('test_ct_python', filename)
mod = importlib.util.module_from_spec(spec)

A = mod.DerivedA()
assert A.base_x == 0
assert A.base_y == 0
assert A.base_z == 0
assert A.derived_a == 0

B = mod.DerivedB()

print("Success")