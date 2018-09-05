#pragma once
#include "../DynamicVisitor.hpp"

namespace ct
{
    template<class T>
    struct ArrayContainerTrait: public IContainerTraits
    {
        using base = IContainerTraits;
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
    private:
        T* m_ptr;
        size_t m_size;
    };
}
