#!/usr/bin/env python3
"""Test that enum types are correctly exposed with expected types."""

import sys
import os

# Allow the built module to be found
base_dir = os.path.dirname(os.path.abspath(__file__))
candidate_paths = [
    os.path.join(base_dir, '..', '..', 'build', 'tests', 'python', 'libtest_ct_python.so'),
]
for candidate in candidate_paths:
    if os.path.exists(candidate):
        sys.path.insert(0, os.path.dirname(candidate))
        break

try:
    import importlib.util
    import test_ct_python as bindings
except Exception:
    raise RuntimeError("Failed to load libtest_ct_python bindings")

def test_my_enum_is_int_convertible():
    """Check that MyEnum members can be converted to int."""
    assert hasattr(bindings, 'MyEnum')
    enum_cls = bindings.MyEnum
    assert isinstance(enum_cls.k2345, enum_cls)
    assert isinstance(int(enum_cls.k2345), int)

def test_standard_enum_has_expected_members():
    """Check that StandardEnum has expected members."""
    assert hasattr(bindings.MyClass, 'StandardEnum')
    enum_cls = bindings.MyClass.StandardEnum
    assert hasattr(enum_cls, 'k0')
    assert hasattr(enum_cls, 'k1')
    assert isinstance(enum_cls.k0, enum_cls)
    assert isinstance(enum_cls.k1, enum_cls)

def test_standard_enum2_has_expected_members():
    """Check that StandardEnum2 has expected members."""
    assert hasattr(bindings.MyClass, 'StandardEnum2')
    enum_cls = bindings.MyClass.StandardEnum2
    assert hasattr(enum_cls, 'k1234')
    assert hasattr(enum_cls, 'kASDF')
    assert isinstance(enum_cls.k1234, enum_cls)
    assert isinstance(enum_cls.kASDF, enum_cls)

def test_with_enum():
    obj = bindings.WithEnum()
    assert isinstance(obj.m_enum, bindings.MyEnum)

