#pragma once
#include <ct/reflect.hpp>
#include <cmath>
#include <vector>

struct ReflectedStruct
{
  public:
    float x;
    float y;
    float z;
    int id;
};

struct Inherited : public ReflectedStruct
{
    double w;
};

struct Composite
{
    ReflectedStruct a;
    ReflectedStruct b;
};

struct TestA
{
    float x, y, z;

    float norm() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }

    TestA mul(int val) const
    {
        return {x*val, y*val, z*val};
    }

    TestA mul(float val, float beta) const
    {
        return {x*val - beta, y*val - beta, z*val - beta};
    }

    TestA mul(float val) const
    {
        return {x*val, y*val, z*val};
    }

    static TestA create()
    {
        return {0,0,0};
    }
};

struct Base
{
    float base_x;
    float base_y;
    float base_z;
};

struct DerivedA: virtual public Base
{
    float derived_a;
};

struct DerivedB: virtual public Base
{
    float derived_b;
};

struct DerivedC: virtual public DerivedA, virtual public DerivedB
{
    float derived_c;
};

struct Wrapper
{
    REFLECT_INTERNAL_START(Wrapper)
        REFLECT_INTERNAL_MEMBER(float, val)
    REFLECT_INTERNAL_END
};

struct TestB
{
    float x, y, z;
};

struct TestC
{
    float y, x, z;
};

struct TestVec
{
    std::vector<float> vec;
};

struct PrivateMutableAccess
{
  public:
    const float& getX() const { return x; }
    float& mutateX() { return x; }
  private:
    float x;
};

struct MultipleInheritance: public PrivateMutableAccess, public ReflectedStruct
{
    float asdf;
};

struct PrivateGetAndSet
{
  public:
    const float& getX() const { return x; }
    void setX(const float val) { x = val; }
  private:
    float x;
};

struct PointerOwner
{
    PointerOwner(const PointerOwner&) = default;
    PointerOwner(PointerOwner&&) = default;
    PointerOwner(ReflectedStruct* ptr_ = nullptr) : ptr(ptr_) {}
    ReflectedStruct* ptr;
};

struct ExplicitThisProperty
{
    float prop;
};

const float& getExplicitThisProperty(const ExplicitThisProperty& data)
{
    return data.prop;
}

float& setExplicitThisProperty(ExplicitThisProperty& data)
{
    return data.prop;
}

struct WeirdWeakOwnerShip
{
    WeirdWeakOwnerShip(const size_t cnt = 10)
    {
        struct_vec.resize(cnt);
        for (size_t i = 0; i < cnt; ++i)
        {
            pointer_owners.push_back(&struct_vec[i]);
        }
    }
    std::vector<ReflectedStruct> struct_vec;
    std::vector<PointerOwner> pointer_owners;
};

// Doesn't work with C++11 :/


struct InternallyReflected
{
    REFLECT_INTERNAL_START(InternallyReflected)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, z)
    REFLECT_INTERNAL_END
};


namespace cereal
{
    template <class AR>
    void serialize(AR&, ReflectedStruct*)
    {
    }
}
