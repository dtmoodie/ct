msvc 2017, gcc 5.4, gcc 7.3: [![Build status](https://ci.appveyor.com/api/projects/status/nuxy51ymuak4ab7x/branch/master?svg=true)](https://ci.appveyor.com/project/dtmoodie/ct/branch/master)

gcc 4.8: [![Build Status](https://travis-ci.org/dtmoodie/ct.svg?branch=master)](https://travis-ci.org/dtmoodie/ct)


ct is a collection of useful compile time utilities for c++.  It originally started as a collection of cool functions that then morphed into a set of tools useful for compile time reflection.

An example of ct's awesome compile time capabilities can be seen with this quick example:


    #include <ct/reflect.hpp>
    #include <ct/reflect/print.hpp>
    #include <iostream>
    
    struct MyStruct
    {
        REFLECT_INTERNAL_START
          REFLECT_INTERNAL_MEMBER(float, member_a, 1.0)
          REFLECT_INTERNAL_MEMBER(float, member_b, 2.0)
          REFLECT_INTERNAL_MEMBER(float, member_c, 3.0)
        REFLECT_INTERNAL_END;
    };
    
    int main()
    {
      MyStruct my_struct;
      std::cout << my_struct << std::endl;
    }
This will produce the following:

    (member_a: 1.0 member_b: 2.0 member_c: 3.0)


The above will automatically generate the std::ostream& operator<< for MyStruct.  ct also has facilities to generate load and save functions for cereal, data hashing, object hashing, and python bindings are on the way.

Likewise json serialization is trivial using cereal.

    #include <ct/reflect/cerealize.hpp>
    #include <cereal/archives/json.hpp>
    int main()
    {
      MyStruct my_struct;
      cereal::JSONOutputArchive ar(std::cout);
      ar(my_struct);
    }

The above will produce the following:

    {
        "value0": {
            "member_a": 1.0,
            "member_b": 2.0,
            "member_c": 3.0
        }
    }


Metadata can be added to each field


    using Description = ct::metadata::Description;
    struct MyStruct
    {
        REFLECT_INTERNAL_START
            REFLECT_INTERNAL_MEMBER(float, member_a, 1.0, Description("Member a description"))
            REFLECT_INTERNAL_MEMBER(float, member_b, 2.0, Description("Member b description"))
            REFLECT_INTERNAL_MEMBER(float, member_c, 3.0, Description("Member c description"))
        REFLECT_INTERNAL_END;
    };

Then the struct can be used with boost program options to load values from the command line.

    ct::po::options_description desc;
    ct::registerOptions(my_struct, desc);
    std::cout << desc << std::endl;

    ct::po::variables_map vm;
    ct::po::store(ct::po::parse_command_line(ac, av, desc), vm);
    ct::po::notify(vm);

    ct::readOptions(my_struct, vm);

The std::cout << desc << std::endl call will produce the following:

    --member_c arg (=3)   Member c description
    --member_b arg (=2)   Member b description
    --member_a arg (=1)   Member a description


The macros used in MyStruct can also be expanded to just a few lines for projects disallowing macro usage.  The following is equivalent to the previous MyStruct.

    struct MyStruct
    {
        float member_a = 1.0;
        float member_b = 2.0;
        float member_c = 3.0;

        constexpr static auto getPtr(Indexer<0>)
        {
            return makeMemberObjectPointer("member_a", &MyStruct::member_a, Description("Member a description"));
        }

        constexpr static auto getPtr(Indexer<1>)
        {
            return makeMemberObjectPointer("member_b", &MyStruct::member_b, Description("Member b description"));
        }

        constexpr static auto getPtr(Indexer<2>)
        {
            return makeMemberObjectPointer("member_c", &MyStruct::member_c, Description("Member c description"));
        }
        static constexpr const ct::index_t NUM_FIELDS = 3;
    };

Furthermore the reflection metadata does not need to be embedded into a type, instead it can be declared externally as such:

    namespace ct
    {
        REFLECT_BEGIN(MyStruct)
            PUBLIC_ACCESS_WITH_METADATA(member_a, Description("Member a description"))
            PUBLIC_ACCESS_WITH_METADATA(member_b, Description("Member b description"))
            PUBLIC_ACCESS_WITH_METADATA(member_c, Description("Member c description"))
        REFLECT_END;
    }

Lastly properties with get and set methods are supported seamlessly the same as publically accessible data, and member functions can be reflected about.
