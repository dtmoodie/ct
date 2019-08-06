#ifndef CT_INTEROP_BOOST_PYTHON_TENSOR_CONVERTER_HPP
#define CT_INTEROP_BOOST_PYTHON_TENSOR_CONVERTER_HPP
#include "ReflectedConverter.hpp"

#include <ct/concepts.hpp>
#include <ct/concepts/tensor.hpp>

#include <boost/python/numpy.hpp>
namespace ct
{
    template<class T>
    struct TensorConverter
    {
        static std::string repr(const T& obj);

        static void registerToPython(const char* name);

        static bool convertFromPython(const boost::python::object&, T&);

        static boost::python::object convertToPython(const T& result);
    };

    template<class T>
    struct TensorConverter<std::shared_ptr<T>>
    {
        static std::string repr(const T& obj);

        static void registerToPython(const char* name);

        static bool convertFromPython(const boost::python::object&, std::shared_ptr<T>&);

        static boost::python::object convertToPython(const std::shared_ptr<T>& result);
    };

    template <class T>
    struct ReflectedConverter<T, 3, EnableIf<conceptCheck<T, TensorConcept>()>>: TensorConverter<T>
    {

    };

    template <class T>
    struct ReflectedConverter<std::shared_ptr<T>, 3, EnableIf<conceptCheck<T, TensorConcept>()>>: TensorConverter<std::shared_ptr<T>>
    {

    };

    ///////////////////////////////////////////////////////////////////////////////////////
    /// IMPLEMENTATION
    ///////////////////////////////////////////////////////////////////////////////////////
    // TODO: implement conversion to numpy

    template <class T>
    std::string TensorConverter<T>::repr(const T& obj)
    {
        return {};
    }

    template <class T>
    void TensorConverter<T>::registerToPython(const char* name)
    {
        boost::python::class_<T> bpobj(name);
        detail::addProperties<T>(bpobj);
        detail::addInit<T>(bpobj);
    }

    template <class T>
    bool TensorConverter<T>::convertFromPython(const boost::python::object&, T&)
    {
        return false;
    }

    template <class T>
    boost::python::object TensorConverter<T>::convertToPython(const T& result)
    {
        return {};
    }
}

#endif // CT_INTEROP_BOOST_PYTHON_TENSOR_CONVERTER_HPP
