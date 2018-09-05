#pragma once
#include "../DynamicVisitor.hpp"

namespace ct
{
    template<class T>
    struct TTraits<std::shared_ptr<T>, void>: public IStructTraits
    {
        virtual void visit(IDynamicVisitor* visitor) override
        {
            bool empty = true;
            auto visitor_trait = visitor->traits();
            if(visitor_trait.reader)
            {
                (*visitor)(&empty, "empty");
                if(!empty)
                {

                }
            }else
            {
                empty = ((*m_ptr) == nullptr);
                (*visitor)(&empty, "empty");
                if(!empty)
                {

                }
            }
        }

        virtual size_t size() const override
        {

        }
        virtual bool triviallySerializable() const override
        {

        }
        virtual bool isPrimitiveType() const override
        {
            return false;
        }
        virtual const void* ptr() const override
        {
            return nullptr;
        }
        virtual void* ptr() override
        {
            return nullptr;
        }
    private:
        std::shared_ptr<T>* m_ptr;
    };
}
