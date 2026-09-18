import importlib.util
import os

import pytest


def load_bindings():
    # The module is built as test_ct_python.so (no "lib" prefix, see
    # CMakeLists.txt), so when ctest runs pytest from the build directory it
    # is simply importable by name. Otherwise fall back to loading it by
    # explicit path.
    try:
        import test_ct_python
        return test_ct_python
    except ImportError:
        pass

    here = os.path.dirname(os.path.abspath(__file__))
    candidates = [
        os.path.join(here, 'test_ct_python.so'),
        os.path.join(here, '..', '..', 'build', 'tests', 'python', 'test_ct_python.so'),
    ]
    for filename in candidates:
        if os.path.exists(filename):
            spec = importlib.util.spec_from_file_location('test_ct_python', filename)
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)
            return mod
    raise ImportError('test_ct_python module not found; build the python tests first')


@pytest.fixture(scope="session")
def bindings():
    return load_bindings()
