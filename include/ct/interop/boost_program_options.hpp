#ifndef CT_INTEROP_BOOST_PROGRAM_OPTIONS_HPP
#define CT_INTEROP_BOOST_PROGRAM_OPTIONS_HPP
#include <ct/reflect.hpp>
#include <ct/reflect/metadata.hpp>
#include <ct/reflect_traits.hpp>

#include <boost/program_options.hpp>

namespace ct
{
    namespace po = boost::program_options;

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
        if (field_desc)
        {
            desc.add_options()(path.c_str(), po::value<T>()->default_value(field), field_desc->m_desc);
        }
        else
        {
            desc.add_options()(path.c_str(), po::value<T>()->default_value(field));
        }
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
        const auto& field = ptr.get(obj);
        if (!path.empty())
        {
            path += '.';
        }
        const auto field_name = getName<I, T>();
        path += field_name.toString();
        auto field_desc = ptr.getMetadata().template getMetadata<ct::Description>();
        registerWithMetadata(field, desc, field_desc, path);
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
    DisableIfReflected<T> registerOption(T& obj, po::variables_map& vm, const std::string& path)
    {
        if (vm.count(path))
        {
            obj = vm[path].as<T>();
        }
    }

    template <class T, index_t I>
    DisableIfReflected<typename std::decay<typename FieldSetType<T, I>::type>::type>
    readField(T& obj, const Indexer<I> idx, po::variables_map& vm, std::string path)
    {
        auto ptr = Reflect<T>::getPtr(idx);
        using type = typename FieldSetType<T, I>::type;
        if (!path.empty())
        {
            path += '.';
        }
        const auto field_name = getName<I, T>();
        path += field_name.toString();

        if (vm.count(path))
        {
            ptr.set(obj, vm[path].as<typename std::decay<type>::type>());
        }
    }

    template <class T, index_t I>
    EnableIfReflected<typename std::decay<typename FieldSetType<T, I>::type>::type>
    readField(T& obj, const Indexer<I> idx, po::variables_map& vm, std::string path)
    {
        auto ptr = Reflect<T>::getPtr(idx);
        if (!path.empty())
        {
            path += '.';
        }
        const auto field_name = getName<I, T>();
        path += field_name.toString();
        readOptions(ptr.set(obj), vm, path);
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
} // namespace ct

#endif // CT_INTEROP_BOOST_PROGRAM_OPTIONS_HPP
