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
    struct IStructTraits
    {
        virtual ~IStructTraits() = default;
        virtual void visit(IDynamicVisitor* visitor) = 0;
        virtual size_t size() const = 0;
        virtual bool triviallySerializable() const = 0;
        virtual const void* ptr() const = 0;
        virtual void* ptr() = 0;
    };

    struct IContainerTraits
    {
        virtual ~IContainerTraits() = default;
        virtual TypeInfo keyType() const = 0;
        virtual TypeInfo valueType() const = 0;
        virtual bool isContinuous() const = 0;
        virtual bool podValues() const = 0;
        virtual bool podKeys() const = 0;
        virtual size_t numKeys() const = 0;
        virtual size_t numValues() const = 0;
    };

    template<class T>
    struct TTraits;

    struct IDynamicVisitor
    {
        virtual ~IDynamicVisitor(){}
        template<class T>
        IDynamicVisitor& operator()(T* val, const std::string& name = "", const size_t cnt = 1);
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

        virtual IDynamicVisitor& startContainer(IContainerTraits&) = 0;
        virtual IDynamicVisitor& endContainer() = 0;

        virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "", const size_t cnt = 1)
        {
            namespaces.push_back(name.c_str());
            for(size_t i = 0; i < cnt; ++i)
            {
                val[i].visit(this);
            }
            namespaces.pop_back();
            return *this;
        }

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
    struct TTraits<std::vector<T>>: public IContainerTraits
    {
        TTraits(const std::vector<T>& vec):
            m_vec(vec)
        {
        }
        virtual TypeInfo keyType() const override
        {
            return TypeInfo(typeid(void));
        }
        virtual TypeInfo valueType() const override
        {
            return TypeInfo(typeid(T));
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
            return m_vec.size();
        }
    private:
        const std::vector<T>& m_vec;
    };

    template<class T>
    TTraits<T> makeTraits(T& container)
    {
        return TTraits<T>(container);
    }

    template<class T>
    void visit(IDynamicVisitor& visitor, std::vector<T>* val, const std::string& name = "", const size_t cnt = 1)
    {
        auto traits = makeTraits(*val);
        visitor.startContainer(traits);
        visitor(val->data(), name, val->size());
        visitor.endContainer();
    }

    template<class T>
    IDynamicVisitor& IDynamicVisitor::operator()(T* val, const std::string& name, const size_t cnt)
    {
        visit(*this, val, name, cnt);
        return *this;
    }

}

