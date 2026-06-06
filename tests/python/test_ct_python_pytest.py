#!/usr/bin/env python3
"""
Pytest version of the Python bindings test that covers all enum functionality.
This test file can be run with pytest or as a regular Python script.
"""

import importlib.util
import os
import sys
import test_ct_python as mod

def test_reflected_struct():
    """Test basic struct creation."""
    obj = mod.ReflectedStruct(x=1, y=2, z=3, id=10)
    assert obj.x == 1.0
    assert obj.y == 2.0
    assert obj.z == 3.0
    assert obj.id == 10

def test_enum_functionality():
    """Test enum functionality."""
    obj_enum = mod.WithEnum()
    val = obj_enum.m_enum
    assert val is not None

def test_my_enum_values():
    """Test MyEnum values."""
    val = mod.MyEnum.k2345
    assert val is not None
    assert int(val) == 1  # Based on the original test output

def test_standard_enum_values():
    """Test StandardEnum values."""
    val = mod.MyClass.StandardEnum.k0
    assert val is not None
    assert int(val) == 0

    val = mod.MyClass.StandardEnum.k1
    assert val is not None
    assert int(val) == 1

def test_standard_enum2_values():
    """Test StandardEnum2 values."""
    val = mod.MyClass.StandardEnum2.k1234
    assert val is not None
    assert int(val) == 1

    val = mod.MyClass.StandardEnum2.kASDF
    assert val is not None
    assert int(val) == 0

def test_all_enum_assertions():
    """Test that all enum assertions pass (from original test)."""
    # This test verifies that the original test's "all enum assertions passed" works
    # We're essentially checking that the enum values can be accessed and have correct values
    assert hasattr(mod, 'MyEnum')
    assert hasattr(mod.MyClass, 'StandardEnum')
    assert hasattr(mod.MyClass, 'StandardEnum2')

    # Test that we can access enum values
    assert hasattr(mod.MyEnum, 'k2345')
    assert hasattr(mod.MyClass.StandardEnum, 'k0')
    assert hasattr(mod.MyClass.StandardEnum, 'k1')
    assert hasattr(mod.MyClass.StandardEnum2, 'k1234')
    assert hasattr(mod.MyClass.StandardEnum2, 'kASDF')

if __name__ == "__main__":
    # Run tests directly if script is executed
    test_reflected_struct()
    test_enum_functionality()
    test_my_enum_values()
    test_standard_enum_values()
    test_standard_enum2_values()
    test_all_enum_assertions()
    print("All tests passed!")