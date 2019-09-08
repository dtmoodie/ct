#pragma once
#include "Data.hpp"
#include "Reflect.hpp"

#ifdef HAVE_OPENCV
#include <ct/types/opencv.hpp>
#endif


#ifdef HAVE_EIGEN
#include <ct/types/eigen.hpp>
#endif

#include <ct/reflect/compare-container-inl.hpp>
#include <ct/reflect/print.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <vector>

template <class T>
void mulImpl(T& obj, const ct::Indexer<0> idx)
{
    auto accessor = ct::Reflect<T>::getPtr(idx);
    accessor.set(obj) *= 2;
}

template <class T, ct::index_t I>
void mulImpl(T& obj, const ct::Indexer<I> idx)
{
    auto accessor = ct::Reflect<T>::getPtr(idx);
    accessor.set(obj) *= 2;
    mulImpl(obj, --idx);
}

template <class T>
void mul(T& obj)
{
    mulImpl(obj, ct::Reflect<T>::end());
}

struct DebugEqual
{
    template <class T>
    auto test(const char* name, const T& lhs, const T& rhs) const ->
        typename std::enable_if<!std::is_array<T>::value, bool>::type
    {
        if (lhs != rhs)
        {
            std::cout << name << " values not equal: " << lhs << " != " << rhs << std::endl;
            return false;
        }
        return true;
    }

    template <class T, size_t N>
    bool test(const char* name, const T (&lhs)[N], const T (&rhs)[N]) const
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (lhs[i] != rhs[i])
            {
                std::cout << name << " values not equal (" << i << "): " << lhs << " != " << rhs << std::endl;
                return false;
            }
        }

        return true;
    }

    template <class T>
    bool test(const char* name, const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const
    {
        if (lhs != rhs)
        {
            std::cout << name << " values not equal: " << lhs << " != " << rhs << std::endl;
            return false;
        }
        return true;
    }

    template <class T>
    bool test(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const
    {
        if ((lhs && rhs) && (*lhs != *rhs))
        {
            std::cout << " values not equal: " << lhs << " != " << rhs << std::endl;
            return false;
        }
        return true;
    }
};

template <class T>
struct TestData;

#define TEST_DATA(TYPE, ...)                                                                                           \
    template <>                                                                                                        \
    struct TestData<TYPE>                                                                                              \
    {                                                                                                                  \
        static TYPE init() { return __VA_ARGS__; }                                                                     \
    }

TEST_DATA(ReflectedStruct, {0, 1, 2, 3});
TEST_DATA(Composite, {{0, 1, 2, 3}, {4, 5, 6, 7}});
TEST_DATA(TestA, {0, 1, 2});
TEST_DATA(TestB, {0, 1, 2});
TEST_DATA(TestC, {0, 1, 2});
TEST_DATA(TestVec, {{0, 1, 2, 3, 4}});
TEST_DATA(PrivateMutableAccess, {4});
TEST_DATA(InternallyReflected, {5, 10, 15});
TEST_DATA(PrivateGetAndSet, {5.2f});
TEST_DATA(std::vector<ReflectedStruct>, {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}});
using MapVec = std::map<int, TestVec>;
TEST_DATA(MapVec, {{0, {{0, 1, 2, 3, 4, 5}}}, {10, {{0, 3, 4, 5}}}});
TEST_DATA(Wrapper, {5});

template <>
struct TestData<Inherited>
{
    static Inherited init()
    {
        Inherited ret;
        ret.x = 0.0f;
        ret.y = 1.0f;
        ret.z = 2.0f;
        ret.id = 3;
        ret.w = 4.0;
        return ret;
    }
};

using StringMap = std::map<std::string, Inherited>;
template <>
struct TestData<StringMap>
{
    static StringMap init()
    {
        StringMap ret;
        Inherited val;
        val.x = 0;
        val.y = 1;
        val.z = 2;
        val.id = -1;
        val.w = 15;
        ret["asdf"] = val;
        val.z = 4;
        ret["asdfg"] = val;

        return ret;
    }
};

template <>
struct TestData<MultipleInheritance>
{
    static MultipleInheritance init()
    {
        MultipleInheritance asdf;
        asdf.ReflectedStruct::x = 0;
        asdf.ReflectedStruct::y = 1;
        asdf.ReflectedStruct::z = 2;
        asdf.id = 3;
        asdf.mutateX() = 4;
        asdf.asdf = 5;
        return asdf;
    }
};

TEST_DATA(ExplicitThisProperty, {});
template <>
struct TestData<DerivedA>
{
    static DerivedA init()
    {
        DerivedA derived;
        derived.base_x = 0;
        derived.base_y = 1;
        derived.base_z = 2;
        derived.derived_a = 3;
        return derived;
    }
};
template <>
struct TestData<DerivedB>
{
    static DerivedB init()
    {
        DerivedB derived;
        derived.base_x = 0;
        derived.base_y = 1;
        derived.base_z = 2;
        derived.derived_b = 4;
        return derived;
    }
};

template <>
struct TestData<DerivedC>
{
    static DerivedC init()
    {
        DerivedC derived;
        derived.base_x = 0;
        derived.base_y = 1;
        derived.base_z = 2;
        derived.derived_a = 3;
        derived.derived_b = 4;
        derived.derived_c = 5;
        return derived;
    }
};
TEST_DATA(Virtual, {});
TEST_DATA(Templated<double>, {});

#ifdef HAVE_OPENCV
TEST_DATA(cv::Vec2f, {2, 3});
TEST_DATA(cv::Rect2f, {0, 1, 2, 3});
TEST_DATA(cv::Rect, {0, 1, 2, 3});
TEST_DATA(cv::Point2f, {0, 1});
TEST_DATA(cv::Point, {0, 1});
TEST_DATA(cv::Point3f, {0, 1, 2});
TEST_DATA(cv::Scalar, {0, 1, 2, 3});
TEST_DATA(cv::Mat_<float>, cv::Mat_<float>::eye(4, 4));
TEST_DATA(cv::Mat_<cv::Vec3f>, cv::Mat_<cv::Vec3f>::ones(4, 4) * 3.14159);
#endif

#ifdef HAVE_EIGEN
using EigenStaticMatrix = Eigen::Matrix<float, 3, 3>;
using EigenDynamicMatrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
TEST_DATA(Eigen::Matrix3f, Eigen::Matrix3f::Identity());
TEST_DATA(Eigen::MatrixXf, Eigen::MatrixXf::Identity(5, 5));
#endif

using TestTypes = ct::VariadicTypedef<ReflectedStruct,
                                      Inherited,
                                      Composite,
                                      TestA, TestB, TestC,
                                      TestVec,
                                      DerivedA, DerivedB, DerivedC,
                                      PrivateMutableAccess,
                                      InternallyReflected,
                                      PrivateGetAndSet,
                                      MultipleInheritance,
                                      ExplicitThisProperty,
                                      Virtual,
                                      std::vector<ReflectedStruct>,
                                      std::map<std::string, Inherited>
#ifdef HAVE_OPENCV
                                      ,
                                      cv::Vec2f,
                                      cv::Rect2f,
                                      cv::Rect,
                                      cv::Point2f,
                                      cv::Point,
                                      cv::Point3f,
                                      cv::Scalar,
                                      cv::Mat_<float>,
                                      cv::Mat_<cv::Vec3f>
#endif
#ifdef HAVE_EIGEN
#if !(defined(_MSC_VER) && _MSC_VER == 1900)
                                      ,
                                      Eigen::Matrix3f,
                                      Eigen::MatrixXf
#endif
#endif
                                      >;

template <class T>
struct ToTestTypes;

template <class... Ts>
struct ToTestTypes<ct::VariadicTypedef<Ts...>>
{
    using type = ::testing::Types<Ts...>;
};

