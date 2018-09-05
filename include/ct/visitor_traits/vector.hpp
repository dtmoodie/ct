#pragma once
#include "../DynamicVisitor.hpp"

namespace ct
{
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
}
