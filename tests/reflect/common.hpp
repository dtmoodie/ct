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

template <class Tester>
void testTypes(Tester& tester)
{
    {
        ReflectedStruct data{0, 1, 2, 3};
        tester.test(data);
    }
    {
        Inherited data;
        data.id = 3;
        data.w = 4;
        data.x = 0;
        data.y = 1;
        data.z = 2;
        tester.test(data);
    }
    {
        Composite data{{0, 1, 2, 3}, {4, 5, 6, 7}};
        tester.test(data);
    }
    {
        TestA data{0, 1, 2};
        tester.test(data);
    }
    {
        TestB data{0, 1, 2};
        tester.test(data);
    }
    {
        TestC data{0, 1, 2};
        tester.test(data);
    }
    {
        TestVec data;
        data.vec = {0, 1, 2, 3, 4};
        tester.test(data);
    }
    {
        PrivateMutableAccess pma;
        pma.mutateX() = 4;
        tester.test(pma);
    }
    {
        InternallyReflected data;
        data.x = 5;
        data.y = 10;
        data.z = 15;
        tester.test(data);
    }
    {
        PrivateGetAndSet pgs;
        pgs.setX(5.2f);
        tester.test(pgs);
    }
    {
        std::vector<ReflectedStruct> vec{{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}};
        tester.test(vec);
    }
    {
        std::map<std::string, Inherited> map;
        Inherited obj;
        obj.id = 0;
        obj.w = 0;
        obj.x = 1;
        obj.y = 2;
        obj.z = 3;

        map["asdf"] = obj;
        obj.z = 4;
        map["asdfg"] = obj;
        tester.test(map);
    }
    {
        std::map<int, TestVec> mapvec;
        TestVec obj;
        obj.vec = {0, 1, 2, 3, 4, 5};
        mapvec[0] = obj;
        obj.vec = {0, 3, 4, 5};
        mapvec[10] = obj;
        tester.test(mapvec);
    }
    {
        std::shared_ptr<ReflectedStruct> data = std::make_shared<ReflectedStruct>();
        tester.test(data);
    }
    {
        Wrapper data{5};
        tester.test(data);
    }
    {
        // WeirdWeakOwnerShip data;
        // tester.test(data);
    } {
        MultipleInheritance asdf;
        asdf.ReflectedStruct::x = 0;
        asdf.ReflectedStruct::y = 1;
        asdf.ReflectedStruct::z = 2;
        asdf.id = 3;
        asdf.mutateX() = 4;
        asdf.asdf = 5;
        tester.test(asdf);
    }
    {
        ExplicitThisProperty data;
        tester.test(data);
    }
    {
        DerivedC derived;
        derived.base_x = 0;
        derived.base_y = 1;
        derived.base_z = 2;
        derived.derived_a = 3;
        derived.derived_b = 4;
        derived.derived_c = 5;

        tester.test(derived);
    }
    {
        Virtual virt;
        tester.test(virt);
    }
    {
        Templated<double> data;
        tester.test(data);
    }
#ifdef HAVE_OPENCV
    {
        cv::Vec2f data(2, 3);
        tester.test(data);
    }
    {
        cv::Rect2f data(0, 1, 2, 3);
        tester.test(data);
    }
    {
        cv::Rect data(0, 1, 2, 3);
        tester.test(data);
    }
    {
        cv::Point2f data(0, 1);
        tester.test(data);
    }

    {
        cv::Point data(0, 1);
        tester.test(data);
    }
    {
        cv::Point3f data(0, 1, 2);
        tester.test(data);
    }

    {
        cv::Scalar data(0, 1, 2, 3);
        tester.test(data);
    }
    {
        cv::Mat_<float> mat = cv::Mat_<float>::eye(4, 4);
        tester.test(mat);
    }

    {
        cv::Mat_<cv::Vec3f> mat = cv::Mat_<cv::Vec3f>::zeros(4, 4);
        mat += 1;
        mat *= 3.14159;
        tester.test(mat);
    }
    {
        // cv::Mat mat(CV_32F, 4, 4);
        // tester.test(mat);
    }
#endif

#ifdef HAVE_EIGEN
#if !(defined(_MSC_VER) && _MSC_VER == 1900)
    {
        Eigen::Matrix<float, 3, 3> mat = Eigen::Matrix<float, 3, 3>::Identity();
        tester.test(mat);
    }
    {
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> mat =
            Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>::Identity(5, 5);
        tester.test(mat);
    }
#endif
#endif
}

namespace ct
{
#ifdef HAVE_OPENCV
    DECL_NAME(cv::Rect, Rect);
    DECL_NAME(cv::Rect2f, Rectf);
#endif
}
