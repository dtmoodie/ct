#ifndef CT_INTEROP_BOOST_PYTHON_PYTHON_CONVERTER_HPP
#define CT_INTEROP_BOOST_PYTHON_PYTHON_CONVERTER_HPP
#include <boost/python.hpp>
#include <ct/types.hpp>

namespace ct
{
    template <class T>
    boost::python::object convertToPython(const T& data);

    template<class T>
    void registerToPython();

    template <class T>
    bool convertFromPython(const boost::python::object& obj, T& data);

    template <class T, index_t PRIORITY = 10, class ENABLE = void>
    struct PythonConverter : public PythonConverter<T, PRIORITY - 1, void>
    {
    };

    template <class T>
    struct PythonConverter<T, 0, void>
    {
        static void registerToPython(const char*);
        static bool convertFromPython(const boost::python::object& obj, T& val);
        static boost::python::object convertToPython(const T& result);
    };

    template <index_t PRIORITY>
    struct PythonConverter<void, PRIORITY, void>
    {
        static void registerToPython(const char*) {}
    };

    ///////////////////////////////////////////////////////////////
    ///   IMPLEMENTATION
    ///////////////////////////////////////////////////////////////

    template <class T>
    void PythonConverter<T, 0, void>::registerToPython(const char*)
    {
    }

    template <class T>
    bool PythonConverter<T, 0, void>::convertFromPython(const boost::python::object& obj, T& val)
    {
        boost::python::extract<T> extract(obj);
        if (extract.check())
        {
            val = extract();
            return true;
        }
        return false;
    }

    template <class T>
    boost::python::object PythonConverter<T, 0, void>::convertToPython(const T& result)
    {
        return boost::python::object(result);
    }

    template <class T>
    boost::python::object convertToPython(const T& data)
    {
        return PythonConverter<T>::convertToPython(data);
    }

    template <class T>
    bool convertFromPython(const boost::python::object& obj, T& data)
    {
        return PythonConverter<T>::convertFromPython(obj, data);
    }

    inline void eraseAll(std::string& str, char c)
    {
        auto pos = str.find(c);
        while (pos != std::string::npos)
        {
            str.erase(pos, 1);
            pos = str.find(c);
        }
    }

    inline void replaceAll(std::string& str, char v0, char v1)
    {
        for (auto& c : str)
        {
            if (c == v0)
            {
                c = v1;
            }
        }
    }

    template <class T>
    void registerToPython()
    {
        static bool registered = false;
        if (registered)
        {
            return;
        }
        registered = true;

        std::string name = ct::Reflect<T>:: getName().toString();
        eraseAll(name, ':');
        replaceAll(name, '<', '_');
        replaceAll(name, '>', '_');
        eraseAll(name, ',');
        eraseAll(name, ' ');
        eraseAll(name, '-');

        PythonConverter<T>::registerToPython(name.c_str());
    }
}

#endif // CT_INTEROP_BOOST_PYTHON_PYTHON_CONVERTER_HPP
