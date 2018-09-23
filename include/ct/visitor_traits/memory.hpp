#pragma once
#include "../DynamicVisitor.hpp"

namespace ct
{
    template <class T>
    struct TTraits<std::shared_ptr<T>, void> : public IStructTraits
    {
        using base = IStructTraits;

        TTraits(std::shared_ptr<T>* ptr) : m_ptr(ptr) {}
        virtual void visit(IReadVisitor* visitor) override
        {
            uint64_t id = 0;
            auto visitor_trait = visitor->traits();
            if (visitor_trait.reader)
            {
                (*visitor)(&id, "id");
                if (id != 0)
                {
                    auto ptr = visitor->getPointer<T>(id);
                    if (!ptr)
                    {
                        *m_ptr = std::make_shared<T>();
                        (*visitor)(m_ptr->get(), "data");
                        visitor->setSerializedPointer(m_ptr->get(), id);
                        auto cache_ptr = *m_ptr;
                        visitor->pushCach(std::move(cache_ptr), std::string("shared_ptr ") + typeid(T).name(), id);
                    }
                    else
                    {
                        auto cache_ptr =
                            visitor->popCache<std::shared_ptr<T>>(std::string("shared_ptr ") + typeid(T).name(), id);
                        if (cache_ptr)
                        {
                            *m_ptr = cache_ptr;
                        }
                    }
                }
            }
        }

        virtual void visit(IWriteVisitor* visitor) const override
        {
            uint64_t id = 0;
            auto visitor_trait = visitor->traits();

            id = uint64_t(m_ptr->get());
            auto ptr = visitor->getPointer<T>(id);
            (*visitor)(&id, "id");
            if (*m_ptr && ptr == nullptr)
            {
                (*visitor)(m_ptr->get(), "data");
            }
        }

        virtual size_t size() const override { return sizeof(T*); }
        virtual bool triviallySerializable() const override { return false; }
        virtual bool isPrimitiveType() const override { return false; }
        virtual TypeInfo type() const override { return TypeInfo(typeid(T)); }
        virtual const void* ptr() const override { return nullptr; }
        virtual void* ptr() override { return nullptr; }
      private:
        std::shared_ptr<T>* m_ptr;
    };

    template <class T>
    struct TTraits<const std::shared_ptr<T>, void> : public IStructTraits
    {
        using base = IStructTraits;

        TTraits(const std::shared_ptr<T>* ptr) : m_ptr(ptr) {}
        virtual void visit(IReadVisitor*) override {}

        virtual void visit(IWriteVisitor* visitor) const override
        {
            uint64_t id = 0;
            auto visitor_trait = visitor->traits();

            id = uint64_t(m_ptr->get());
            auto ptr = visitor->getPointer<T>(id);
            (*visitor)(&id, "id");
            if (*m_ptr && ptr == nullptr)
            {
                (*visitor)(m_ptr->get(), "data");
            }
        }

        virtual size_t size() const override { return sizeof(T*); }
        virtual bool triviallySerializable() const override { return false; }
        virtual bool isPrimitiveType() const override { return false; }
        virtual TypeInfo type() const override { return TypeInfo(typeid(T)); }
        virtual const void* ptr() const override { return nullptr; }
        virtual void* ptr() override { return nullptr; }
      private:
        const std::shared_ptr<T>* m_ptr;
    };

    template <class T>
    struct TTraits<T*, void> : public IStructTraits
    {
        using base = IStructTraits;

        TTraits(T** ptr) : m_ptr(ptr) {}

        virtual void visit(IReadVisitor* visitor) override
        {
            uint64_t id = 0;
            auto visitor_trait = visitor->traits();

            (*visitor)(&id, "id");
            if (id != 0)
            {
                auto ptr = visitor->getPointer<T>(id);
                if (!ptr)
                {
                    *m_ptr = new T();
                    (*visitor)(*m_ptr, "data");
                    visitor->setSerializedPointer(*m_ptr, id);
                }
                else
                {
                    *m_ptr = ptr;
                }
            }
        }

        virtual void visit(IWriteVisitor* visitor) const override
        {
            uint64_t id = 0;
            auto visitor_trait = visitor->traits();

            id = uint64_t(*m_ptr);
            auto ptr = visitor->getPointer<T>(id);
            (*visitor)(&id, "id");
            if (*m_ptr && ptr == nullptr)
            {
                (*visitor)(*m_ptr, "data");
            }
        }

        virtual size_t size() const override { return sizeof(T*); }
        virtual bool triviallySerializable() const override { return false; }
        virtual bool isPrimitiveType() const override { return false; }
        virtual TypeInfo type() const override { return TypeInfo(typeid(T)); }
        virtual const void* ptr() const override { return nullptr; }
        virtual void* ptr() override { return nullptr; }
      private:
        T** m_ptr;
    };

    template <class T>
    struct TTraits<T* const, void> : public IStructTraits
    {
        using base = IStructTraits;

        TTraits(T* const* ptr) : m_ptr(ptr) {}

        virtual void visit(IReadVisitor*) override {}

        virtual void visit(IWriteVisitor* visitor) const override
        {
            uint64_t id = 0;
            auto visitor_trait = visitor->traits();

            id = uint64_t(*m_ptr);
            auto ptr = visitor->getPointer<T>(id);
            (*visitor)(&id, "id");
            if (*m_ptr && ptr == nullptr)
            {
                (*visitor)(*m_ptr, "data");
            }
        }

        virtual size_t size() const override { return sizeof(T*); }
        virtual bool triviallySerializable() const override { return false; }
        virtual bool isPrimitiveType() const override { return false; }
        virtual TypeInfo type() const override { return TypeInfo(typeid(T)); }
        virtual const void* ptr() const override { return nullptr; }
        virtual void* ptr() override { return nullptr; }
      private:
        T* const* m_ptr;
    };
}
