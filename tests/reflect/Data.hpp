#pragma once
#include <cmath>
#include <ct/reflect.hpp>
#include <vector>

struct ReflectedStruct
{
  public:
    static std::string description() { return "Reflected struct"; }
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
enum MyEnum : uint32_t
{
    k1234,
    k2345,
    k3456
};

struct WithEnum
{

    MyEnum m_enum;
};

struct TestA
{
    float x, y, z;

    float norm() const { return std::sqrt(x * x + y * y + z * z); }

    TestA mul(int val) const { return {x * val, y * val, z * val}; }

    TestA mul(float val, float beta) const { return {x * val - beta, y * val - beta, z * val - beta}; }

    TestA mul(float val) const { return {x * val, y * val, z * val}; }

    static TestA create() { return {0, 0, 0}; }
};

struct Base
{
    float base_x;
    float base_y;
    float base_z;
};

struct DerivedA : virtual public Base
{
    float derived_a;
};

struct DerivedB : virtual public Base
{
    float derived_b;
};

struct DerivedC : virtual public DerivedA, virtual public DerivedB
{
    float derived_c;
};

struct Wrapper
{
    REFLECT_INTERNAL_BEGIN(Wrapper)
        REFLECT_INTERNAL_MEMBER(float, val)
    REFLECT_INTERNAL_END;
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
    PrivateMutableAccess(float v = 0) : m_private_property(v) {}
    const float& getX() const { return m_private_property; }
    float& mutateX() { return m_private_property; }

  private:
    float m_private_property;
};

struct MultipleInheritance : public PrivateMutableAccess, public ReflectedStruct
{
    float asdf;
};

struct PrivateGetAndSet
{
  public:
    PrivateGetAndSet(float v = 0.0) : x(v) {}
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

struct Virtual
{
    virtual ~Virtual();
    REFLECT_INTERNAL_BEGIN(Virtual)
        INTERNAL_MEMBER_FUNCTION(foo)
    REFLECT_INTERNAL_END;
    virtual void foo();
};

Virtual::~Virtual() {}
void Virtual::foo() {}

template <class DType>
struct Templated
{
    DType x;
    DType y;
    DType z;
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

struct InternallyReflected
{
    float foo() const { return 0; }
    float foo(float v) const { return v; }

    REFLECT_INTERNAL_BEGIN(InternallyReflected)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, z)
        MEMBER_FUNCTION(foo,
                        ct::selectConstMemberFunctionPointer<InternallyReflected, float>(&InternallyReflected::foo))
        MEMBER_FUNCTION(
            foo, ct::selectConstMemberFunctionPointer<InternallyReflected, float, float>(&InternallyReflected::foo))
    REFLECT_INTERNAL_END;
};
