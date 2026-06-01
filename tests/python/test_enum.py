import sys
import os

# Add the path to the built module
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import test_ct_python as test_module
    print("Import successful")

    # Test that we can access the enum values
    print("Testing enum access...")

    # Try to access a standard enum
    try:
        # This should work if enums are properly registered
        val = test_module.MyClass.StandardEnum.k0
        print(f"Accessed StandardEnum.k0: {val}")

        # Try to create a new enum value from Python
        new_val = test_module.MyClass.StandardEnum.k1
        print(f"Created StandardEnum.k1: {new_val}")

        # Test conversion back to Python
        print(f"Type of k1: {type(new_val)}")

        print("Enum test completed successfully")

    except Exception as e:
        print(f"Error with enum access: {e}")
        import traceback
        traceback.print_exc()

except ImportError as e:
    print(f"Failed to import test module: {e}")
    import traceback
    traceback.print_exc()