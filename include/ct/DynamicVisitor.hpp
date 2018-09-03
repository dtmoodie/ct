#pragma once
#include "TypeInfo.hpp"
#include <cstdint>
#include <string>
#include <type_traits>
#include <memory>
#include <vector>
#include <sstream>
namespace ct
{
    struct IDataContainer
    {
        TypeInfo type;
    };

    template<class T>
    struct DataContainer: public IDataContainer
    {
        DataContainer(T&& val):
            m_val(std::move(val))
        {
            type = type.template create<T>();
        }
        T m_val;
    };

    struct IDynamicVisitor;
    struct ITraits
    {
        virtual ~ITraits() = default;
        virtual void visit(IDynamicVisitor* visitor) = 0;
    };

    struct IStructTraits: public ITraits
    {
        virtual size_t size() const = 0;
        virtual bool triviallySerializable() const = 0;
        virtual bool isPrimitiveType() const = 0;
        virtual const void* ptr() const = 0;
        virtual void* ptr() = 0;
    };

    struct IContainerTraits: public ITraits
    {
        virtual TypeInfo keyType() const = 0;
        virtual TypeInfo valueType() const = 0;
        virtual TypeInfo containerType() const = 0;
        virtual bool isContinuous() const = 0;
        virtual bool podValues() const = 0;
        virtual bool podKeys() const = 0;
        virtual size_t numKeys() const = 0;
        virtual size_t numValues() const = 0;
        virtual void setKeys(const size_t num) = 0;
        virtual void setValues(const size_t num) = 0;
    };

    template<class T>
    struct ArrayContainerTrait: public IContainerTraits
    {
        T* m_ptr;
        size_t m_size;
        ArrayContainerTrait(T* ptr, const size_t size):
            m_ptr(ptr),
            m_size(size)
        {}
        virtual void visit(IDynamicVisitor* visitor) override
        {
            for(size_t i = 0; i < m_size; ++i)
            {
                (*visitor)(m_ptr + i);
            }
        }
        virtual TypeInfo keyType() const override
        {
            return TypeInfo(typeid(void));
        }
        virtual TypeInfo valueType() const override
        {
            return TypeInfo(typeid(T));
        }

        virtual TypeInfo containerType() const override
        {
            return TypeInfo(typeid(T[]));
        }

        virtual bool isContinuous() const override
        {
            return true;
        }
        virtual bool podValues() const override
        {
            return std::is_pod<T>::value;
        }
        virtual bool podKeys() const override
        {
            return false;
        }
        virtual size_t numKeys() const override
        {
            return 0U;
        }
        virtual size_t numValues() const override
        {
            return m_size;
        }
        void setKeys(const size_t)
        {

        }
        void setValues(const size_t)
        {

        }
    };

    template<class T, class E = void>
    struct TTraits;

    template<class T, class E = void>
    struct IsPrimitive: public std::false_type
    {
    };

    template<class T>
    struct IsPrimitive<T, typename std::enable_if<std::is_same<T, int8_t>::value ||
            std::is_same<T, uint8_t>::value ||
            std::is_same<T, int16_t>::value ||
            std::is_same<T, uint16_t>::value ||
            std::is_same<T, int32_t>::value ||
            std::is_same<T, uint32_t>::value ||
            std::is_same<T, int64_t>::value ||
            std::is_same<T, uint64_t>::value ||
            std::is_same<T, float>::value ||
            std::is_same<T, double>::value ||
            std::is_same<T, void>::value>::type>: public std::true_type
    {

    };

    struct VisitorTraits
    {
        // if true, the name field of the () operator is used to search for the provided data
        bool supports_named_access;
        // If this is true, read data from external source and put into the visited struct
        // else read data from struct and put into output
        bool reader;
    };

    struct IDynamicVisitor
    {
        virtual ~IDynamicVisitor(){}
        template<class T>
        IDynamicVisitor& operator()(T* val, const std::string& name = "", const size_t cnt = 1);
        virtual IDynamicVisitor& operator()(char* val,           const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(int8_t* val,         const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(uint8_t* val,        const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(int16_t* val,        const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(uint16_t* val,       const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(int32_t* val,        const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(uint32_t* val,       const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(int64_t* val,        const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(uint64_t* val,       const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(float* val,          const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(double* val,         const std::string& name = "", const size_t cnt = 1) = 0;
        virtual IDynamicVisitor& operator()(void* binary,        const std::string& name = "", const size_t num_bytes = 1) = 0;
        virtual VisitorTraits traits() const = 0;

        virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "")
        {
            namespaces.push_back(name.c_str());
            val->visit(this);
            namespaces.pop_back();
            return *this;
        }

        virtual IDynamicVisitor& operator()(IContainerTraits* val, const std::string& name = "") = 0;

        template<class T>
        void pushCach(T&& val, const std::string& name)
        {
            auto& container = accessCache(name);
            container = std::unique_ptr<IDataContainer>(new DataContainer<T>(std::move(val)));
        }

        template<class T>
        bool tryPopCache(T& val, const std::string& name)
        {
            auto& container = accessCache(name);
            if(container)
            {
                auto typed = dynamic_cast<DataContainer<T>*>(container.get());
                if(typed)
                {
                    val = typed->m_val;
                    return true;
                }
            }
            return false;
        }

        template<class T>
        T popCache(const std::string& name)
        {
            T ret;
            if(!tryPopCache<T>(ret, name))
            {
                throw std::runtime_error(name + " not a valid cache entry");
            }
            return ret;
        }

        std::string getNamespace() const
        {
            std::stringstream ss;
            for(size_t i = 0; i < namespaces.size(); ++i)
            {
                if(i != 0) ss << '.';
                ss << namespaces[i];
            }
            return ss.str();
        }
    protected:
        virtual std::unique_ptr<IDataContainer>& accessCache(const std::string& name) = 0;
        std::vector<const char*> namespaces;
    };

    template<class T>
    struct TTraits<std::vector<T>, void>: public IContainerTraits
    {
        using base = IContainerTraits;

        TTraits(std::vector<T>* vec):
            m_vec(vec)
        {
        }

        virtual void visit(IDynamicVisitor* visitor) override
        {
            if(IsPrimitive<T>::value)
            {
                (*visitor)(m_vec->data(), "", m_vec->size());
            }else
            {
                for(size_t i = 0; i < m_vec->size(); ++i)
                {
                    (*visitor)(&(*m_vec)[i]);
                }
            }
        }

        virtual TypeInfo keyType() const override
        {
            return TypeInfo(typeid(void));
        }
        virtual TypeInfo valueType() const override
        {
            return TypeInfo(typeid(T));
        }
        virtual TypeInfo containerType() const {return TypeInfo(typeid(std::vector<T>));}
        virtual bool isContinuous() const override
        {
            return true;
        }
        virtual bool podValues() const override
        {
            return std::is_pod<T>::value;
        }
        virtual bool podKeys() const override
        {
            return false;
        }
        virtual size_t numKeys() const override
        {
            return 0U;
        }
        virtual size_t numValues() const override
        {
            return m_vec->size();
        }
        void setKeys(const size_t)
        {

        }
        void setValues(const size_t num)
        {
            m_vec->resize(num);
        }
    private:
        std::vector<T>* m_vec;
    };

    template<class T>
    TTraits<T> makeTraits(T* val)
    {
        return TTraits<T>(val);
    }

    template<class T>
    IDynamicVisitor& IDynamicVisitor::operator()(T* val, const std::string& name, const size_t cnt)
    {
        if(cnt == 1)
        {
            auto traits = makeTraits(val);
            using base = typename decltype(traits)::base;
            (*this)(static_cast<base*>(&traits), name);
        }else
        {
            ArrayContainerTrait<T> traits(val, cnt);
            (*this)(static_cast<IContainerTraits*>(&traits), name);
        }
        return *this;
    }

}

