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
#if __cplusplus > 201103 || _MSC_VER > 1800

struct InternallyReflected
{
    REFLECT_INTERNAL_START(InternallyReflected)
        REFLECT_INTERNAL_MEMBER(float, x)
        REFLECT_INTERNAL_MEMBER(float, y)
        REFLECT_INTERNAL_MEMBER(float, z)
    REFLECT_INTERNAL_END;
};

#else
struct InternallyReflected
{
    float x, y, z;
};
#endif
namespace cereal
{
    template <class AR>
    void serialize(AR&, ReflectedStruct*)
    {
    }
}
