ct is a collection of useful compile time utilities for c++.  It originally started as a collection of cool functions that then morphed into a set of tools useful for compile time reflection.

An example of ct's awesome compile time capabilities can be seen with this quick example:


    #include <ct/reflect.hpp>
    #include <ct/reflect/print.hpp>
    #include <iostream>
    
    struct MyStruct{
        REFLECT_INTERNAL_START
          REFLECT_INTERNAL_MEMBER(float, member_a, 1.0)
          REFLECT_INTERNAL_MEMBER(float, member_b, 2.0)
          REFLECT_INTERNAL_MEMBER(float, member_c, 3.0)
        REFLECT_INTERNAL_END;
    };
    
    int main(){
      MyStruct my_struct;
      std::cout << my_struct << std::endl;
    }
This will produce the following:
    (member_a: 1.0, member_b: 2.0, member_c: 3.0)


The above will automatically generate the std::ostream& operator<< for MyStruct.  ct also has facilities to generate load and save functions for cereal, data hashing, object hashing, and python bindings are on the way.
