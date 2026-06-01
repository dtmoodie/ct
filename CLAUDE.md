# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Codebase Overview

This is a C++ library called "ct" (compile time utilities) that provides compile-time reflection capabilities for C++. The library enables developers to automatically generate serialization, deserialization, printing, and other functionality for C++ structs and enums at compile time.

## Key Features

- Compile-time reflection for C++ structs
- Automatic generation of ostream operators, serialization, and deserialization
- Support for boost program options integration
- Python binding capabilities via boost.python
- Enum support (standard and bitset enums)
- Integration with cereal for JSON serialization
- CUDA compatibility

## Architecture

The library is organized around the concept of reflection, where structs can be decorated with reflection macros to automatically generate functionality. The core reflection system is in the `include/ct/reflect` directory, with interop modules in `include/ct/interop`.

The boost.python interop functionality is in `include/ct/interop/boost_python/` where the `ReflectedConverter.hpp` file handles the conversion between C++ types and Python objects.

## Build System

The project uses CMake for building. Key build targets include:
- `ctest` - run all tests
- `ctest -R test_ct_enum` - run only enum tests
- `ctest -R test_ct_python` - run Python tests
- `ctest -R test_ct_reflect` - run reflection tests

The library can be built as a static library or with Python bindings when boost.python is available.

## Development Commands

- `cmake .` - configure the build
- `make` - build the project
- `ctest` - run all tests
- `ctest -R test_ct_enum` - run enum-related tests
- `ctest -R test_ct_python` - run Python interop tests
- `make test_ct_enum` - build and run the enum test
- `make test_ct_python` - build and run the Python test

## Python Interop

Python bindings are implemented in the boost_python interop module. The key file is `include/ct/interop/boost_python/ReflectedConverter.hpp` which contains the `PythonConverter` template specializations. The fix for enum conversion was in line 541 where the template specialization was changed from `PythonConverter<T, 3, EnableIfIsEnum<T>>` to `PythonConverter<T, 4, EnableIfIsEnum<T>>` to properly handle enum types in Python.

## Test Structure

Tests are organized in the `tests/` directory with different test suites for different functionality:
- `tests/enum` - enum-related tests
- `tests/reflect` - reflection tests
- `tests/python` - Python interop tests
- `tests/string` - string tests

The test suite uses Google Test framework and can be run with `ctest`.