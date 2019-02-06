#ifndef CT_INTEROP_BOOST_PROGRAM_OPTIONS_HPP
#define CT_INTEROP_BOOST_PROGRAM_OPTIONS_HPP
#include <ct/reflect.hpp>

#include <boost/program_options.hpp>

namespace ct
{
    namespace po = boost::program_options;

    struct Description
    {
        constexpr Description(const char* desc) : m_desc(desc) {}

        const char* m_desc;
    };

    template <class T>
    void registerOptions(const T& obj, po::options_description& desc, const std::string& path = "");

    template <class T>
    void readOptions(T& obj, po::variables_map& vm, const std::string& path = "");

    template <class T, class METADATA>
    DisableIfReflected<T>
    registerWithMetadata(const T& field, po::options_description& desc, const METADATA*, const std::string& path)
    {
        desc.add_options()(path.c_str(), po::value<T>()->default_value(field));
    }

    template <class T>
    DisableIfReflected<T> registerWithMetadata(const T& field,
                                               po::options_description& desc,
                                               const Description* field_desc,
                                               const std::string& path)
    {
        desc.add_options()(path.c_str(), po::value<T>()->default_value(field), field_desc->m_desc);
    }

    template <class T, class METADATA>
    EnableIfReflected<T>
    registerWithMetadata(const T& field, po::options_description& desc, const METADATA*, const std::string& path)
    {
        registerOptions(field, desc, path);
    }

    template <class T, index_t I>
    void registerOption(const T& obj, po::options_description& desc, Indexer<I> idx, std::string path)
    {
        auto ptr = Reflect<T>::getPtr(idx);
        const auto& field = get(ptr, obj);
        if (!path.empty())
        {
            path += '.';
        }
        path += getName<I, T>();
        registerWithMetadata(field, desc, getMetadata(ptr), path);
    }

    template <class T>
    void registerOptionIterator(const T& obj, po::options_description& desc, Indexer<0> idx, const std::string& path)
    {
        registerOption(obj, desc, idx, path);
    }

    template <class T, index_t I>
    void registerOptionIterator(const T& obj, po::options_description& desc, Indexer<I> idx, const std::string& path)
    {
        registerOption(obj, desc, idx, path);
        registerOptionIterator(obj, desc, --idx, path);
    }

    template <class T>
    void registerOptions(const T& obj, po::options_description& desc, const std::string& path)
    {
        registerOptionIterator(obj, desc, Reflect<T>::end(), path);
    }

    template <class T>
    DisableIfReflected<T> registerOption(T& obj, po::variables_map& vm, std::string path)
    {
        if (vm.count(path))
        {
            obj = vm[path].as<T>();
        }
    }

    template <class T, index_t I>
    DisableIfReflected<typename FieldSetType<T, I>::type>
    readField(T& obj, const Indexer<I> idx, po::variables_map& vm, std::string path)
    {
        auto ptr = Reflect<T>::getPtr(idx);
        using type = typename FieldSetType<T, I>::type;
        if (!path.empty())
        {
            path += '.';
        }
        path += getName<I, T>();

        if (vm.count(path))
        {
            set(ptr, obj, vm[path].as<type>());
        }
    }

    template <class T, index_t I>
    EnableIfReflected<typename FieldSetType<T, I>::type>
    readField(T& obj, const Indexer<I> idx, po::variables_map& vm, std::string path)
    {
        auto ptr = Reflect<T>::getPtr(idx);
        using type = typename FieldSetType<T, I>::type;
        if (!path.empty())
        {
            path += '.';
        }
        path += getName<I, T>();
        readOptions(set(ptr, obj), vm, path);
    }

    template <class T>
    void readOptionsIterator(T& obj, po::variables_map& vm, const Indexer<0> idx, const std::string& path)
    {
        readField(obj, idx, vm, path);
    }

    template <class T, index_t I>
    void readOptionsIterator(T& obj, po::variables_map& vm, const Indexer<I> idx, const std::string& path)
    {
        readField(obj, idx, vm, path);
        readOptionsIterator(obj, vm, --idx, path);
    }

    template <class T>
    void readOptions(T& obj, po::variables_map& vm, const std::string& path)
    {
        readOptionsIterator(obj, vm, Reflect<T>::end(), path);
    }
}

#endif // CT_INTEROP_BOOST_PROGRAM_OPTIONS_HPP
