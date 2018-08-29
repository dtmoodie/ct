#include "JSONPrinter.hpp"
#include <ct/reflect.hpp>
#include "../reflect/Data.hpp"
#include "../reflect/Reflect.hpp"





struct Vec
{
    float x, y, z;
};
namespace ct
{
    template<>
    struct TTraits<Vec, void>: IStructTraits
    {
        Vec* m_vec;
        using base = IStructTraits;
        TTraits(Vec* vec):m_vec(vec){}

        virtual size_t size() const
        {
            return sizeof(Vec);
        }
        virtual bool triviallySerializable() const
        {
            return std::is_pod<Vec>::value;
        }
        virtual void visit(ct::IDynamicVisitor* visitor) override
        {
            (*visitor)(&m_vec->x, "x")(&m_vec->y, "y")(&m_vec->z, "z");
        }
        virtual const void* ptr() const
        {
            return m_vec;
        }
        virtual void* ptr()
        {
            return m_vec;
        }
    };

    template<class T, index_t I>
    void visitValue(IDynamicVisitor& visitor, T& obj)
    {
        auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});
        visitor(&accessor.set(obj), Reflect<T>::getName(ct::Indexer<I>{}));
    }

    template<class T>
    void visitHelper(IDynamicVisitor& visitor, T& obj, const Indexer<0>)
    {
        visitValue<T, 0>(visitor, obj);
    }

    template<class T, index_t I>
    void visitHelper(IDynamicVisitor& visitor, T& obj, const Indexer<I>)
    {
        visitHelper(visitor, obj, Indexer<I-1>{});
        visitValue<T, I>(visitor, obj);
    }

    template<class T>
    struct TTraits<T, ct::enable_if_reflected<T>>: public IStructTraits
    {
        using base = IStructTraits;
        T* m_ptr;
        TTraits(T* ptr): m_ptr(ptr){}

        virtual void visit(ct::IDynamicVisitor* visitor) override
        {
            visitHelper(*visitor, *m_ptr, Reflect<T>::end());
        }

        virtual size_t size() const override
        {
            return sizeof(T);
        }

        virtual bool triviallySerializable() const override
        {
            return std::is_pod<T>::value;
        }
        virtual const void* ptr() const
        {
            return m_ptr;
        }
        virtual void* ptr()
        {
            return m_ptr;
        }
    };
}
int main()
{
    {
        ct::DynamicPrintVisitor visitor_impl;
        ct::IDynamicVisitor& visitor = visitor_impl;
        Vec vec{0,1,2};
        visitor(&vec);
        ReflectedStruct s1;
        visitor(&s1);
        ReflectedStruct s2[3];
        visitor(s2, "array", 3);

        Composite c1;
        visitor(&c1, "composite");

    }

    std::cout << std::endl;
}

