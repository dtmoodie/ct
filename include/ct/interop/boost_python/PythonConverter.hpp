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

    template<class T>
    void registerToPythonCreateNamespace(std::string name)
    {
        namespace bp = boost::python;
        auto pos = name.find("::");
        const bool have_namespace = pos != std::string::npos;
        auto bracketed_pos = name.find("<");
        // This is false if for example we have vector<std::string> since the namespace std is within the template arg
        const bool namespace_not_in_template_arg = bracketed_pos > pos;
        if(have_namespace && namespace_not_in_template_arg)
        {
            auto namespace_name = name.substr(0, pos);
            bp::scope current_scope;
            bp::object submodule;
            {
                std::string current_scope_name;
                convertFromPython(current_scope.attr("__name__"), current_scope_name);
                if(current_scope_name.find(namespace_name) == current_scope_name.size() - namespace_name.size())
                {
                    submodule = current_scope;
                }else
                {
                    std::string scope_name = current_scope_name + "." + namespace_name;
                    auto mod = PyImport_AddModule(scope_name.c_str());
                    if(mod == nullptr)
                    {
                        PyObject *ptype, *pvalue, *ptraceback;
                        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
                        PyObject_Print(pvalue, stdout, 0);
                        return;
                    }
                    submodule = bp::object(bp::handle<>(bp::borrowed(mod)));
                }

                bp::scope scope(submodule);
                auto substr = name.substr(pos + 2);
                registerToPythonCreateNamespace<T>(substr);
            }
            current_scope.attr(namespace_name.c_str()) = submodule;

            return;
        }
        eraseAll(name, ':');
        replaceAll(name, '<', '_');
        replaceAll(name, '>', '_');
        eraseAll(name, ',');
        eraseAll(name, ' ');
        eraseAll(name, '-');

        PythonConverter<T>::registerToPython(name.c_str());
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

        std::string name = ct::Reflect<T>::getName().toString();
        registerToPythonCreateNamespace<T>(name);
    }
}

#endif // CT_INTEROP_BOOST_PYTHON_PYTHON_CONVERTER_HPP
