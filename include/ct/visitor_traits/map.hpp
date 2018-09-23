#pragma once
#include "../DynamicVisitor.hpp"
#include "string.hpp"
#include <map>
namespace ct
{

    template <class K, class V>
    struct KVP
    {
        K key;
        V value;

        KVP& operator=(const std::pair<const K, V>& other)
        {
            key = other.first;
            value = other.second;
            return *this;
        }

        KVP& operator=(const std::pair<K, V>& other)
        {
            key = other.first;
            value = other.second;
            return *this;
        }
    };

    template <class T1, class T2>
    struct TTraits<KVP<T1, T2>, void> : public IStructTraits
    {
        using base = IStructTraits;
        KVP<T1, T2>* m_ptr;
        TTraits(KVP<T1, T2>* ptr) : m_ptr(ptr) {}

        virtual void visit(ct::IReadVisitor* visitor) override
        {
            (*visitor)(&m_ptr->key, "key");
            (*visitor)(&m_ptr->value, "value");
        }

        virtual void visit(ct::IWriteVisitor* visitor) const override
        {
            (*visitor)(&m_ptr->key, "key");
            (*visitor)(&m_ptr->value, "value");
        }

        virtual size_t size() const { return sizeof(KVP<T1, T2>); }
        virtual bool triviallySerializable() const { return std::is_pod<T1>::value && std::is_pod<T2>::value; }
        virtual bool isPrimitiveType() const { return false; }
        virtual TypeInfo type() const { return TypeInfo(typeid(KVP<T1, T2>)); }
        virtual const void* ptr() const { return m_ptr; }
        virtual void* ptr() { return m_ptr; }
    };

    template <class T1, class T2>
    struct TTraits<const KVP<T1, T2>, void> : public IStructTraits
    {
        using base = IStructTraits;
        const KVP<T1, T2>* m_ptr;
        TTraits(const KVP<T1, T2>* ptr) : m_ptr(ptr) {}

        virtual void visit(ct::IReadVisitor*) override {}

        virtual void visit(ct::IWriteVisitor* visitor) const override
        {
            (*visitor)(&m_ptr->key, "key");
            (*visitor)(&m_ptr->value, "value");
        }

        virtual size_t size() const { return sizeof(KVP<T1, T2>); }
        virtual bool triviallySerializable() const { return std::is_pod<T1>::value && std::is_pod<T2>::value; }
        virtual bool isPrimitiveType() const { return false; }
        virtual TypeInfo type() const { return TypeInfo(typeid(KVP<T1, T2>)); }
        virtual const void* ptr() const { return m_ptr; }
        virtual void* ptr() { return nullptr; }
    };

    template <class K, class V>
    struct TTraits<std::map<K, V>, void> : public IContainerTraits
    {
        using base = IContainerTraits;
        std::map<K, V>* m_ptr;
        size_t num_to_read = 0;
        TTraits(std::map<K, V>* ptr) : m_ptr(ptr) {}

        virtual void visit(IReadVisitor* visitor_) override
        {
            uint64_t size = m_ptr->size();
            auto& visitor = *visitor_;
            const auto trait = visitor.traits();

            for (size_t i = 0; i < num_to_read; ++i)
            {
                K key;
                V val;
                visitor(&key, "key");
                visitor(&val, "value");
                (*m_ptr)[std::move(key)] = std::move(val);
            }
        }

        virtual void visit(IWriteVisitor* visitor_) const override
        {
            uint64_t size = m_ptr->size();
            auto& visitor = *visitor_;
            const auto trait = visitor.traits();
            for (auto itr = m_ptr->begin(); itr != m_ptr->end(); ++itr)
            {
                KVP<K, V> pair;
                pair = *itr;
                visitor(&pair);
            }
        }

        virtual TypeInfo keyType() const override { return TypeInfo(typeid(K)); }
        virtual TypeInfo valueType() const override { return TypeInfo(typeid(V)); }
        virtual TypeInfo type() const { return TypeInfo(typeid(std::map<K, V>)); }
        virtual bool isContinuous() const override { return false; }
        virtual bool podValues() const override { return std::is_pod<V>::value; }
        virtual bool podKeys() const override { return std::is_pod<K>::value; }
        virtual size_t numKeys() const override { return m_ptr->size(); }
        virtual size_t numValues() const override { return m_ptr->size(); }

        virtual void setKeys(const size_t num) override { num_to_read = num; }
        virtual void setValues(const size_t) override {}
    };

    template <class K, class V>
    struct TTraits<const std::map<K, V>, void> : public IContainerTraits
    {
        using base = IContainerTraits;
        const std::map<K, V>* m_ptr;
        size_t num_to_read = 0;
        TTraits(const std::map<K, V>* ptr) : m_ptr(ptr) {}

        virtual void visit(IReadVisitor*) override {}

        virtual void visit(IWriteVisitor* visitor_) const override
        {
            uint64_t size = m_ptr->size();
            auto& visitor = *visitor_;
            const auto trait = visitor.traits();

            for (auto itr = m_ptr->begin(); itr != m_ptr->end(); ++itr)
            {
                KVP<K, V> pair;
                pair = *itr;
                visitor(&pair);
                // visitor(&itr->second, "value");
            }
        }

        virtual TypeInfo keyType() const override { return TypeInfo(typeid(K)); }
        virtual TypeInfo valueType() const override { return TypeInfo(typeid(V)); }
        virtual TypeInfo type() const { return TypeInfo(typeid(std::map<K, V>)); }
        virtual bool isContinuous() const override { return false; }
        virtual bool podValues() const override { return std::is_pod<V>::value; }
        virtual bool podKeys() const override { return std::is_pod<K>::value; }
        virtual size_t numKeys() const override { return m_ptr->size(); }
        virtual size_t numValues() const override { return m_ptr->size(); }

        virtual void setKeys(const size_t num) override { num_to_read = num; }
        virtual void setValues(const size_t) override {}
    };

    template <class V>
    struct TTraits<std::map<std::string, V>, void> : public IContainerTraits
    {
        using base = IContainerTraits;
        std::map<std::string, V>* m_ptr;
        size_t num_to_read = 0;
        TTraits(std::map<std::string, V>* ptr) : m_ptr(ptr) {}

        virtual void visit(IReadVisitor* visitor_) override
        {
            uint64_t size = m_ptr->size();
            auto& visitor = *visitor_;
            const auto trait = visitor.traits();

            for (size_t i = 0; i < num_to_read; ++i)
            {
                std::string key;
                visitor(&key);
                V val;
                visitor(&val);
                (*m_ptr)[std::move(key)] = std::move(val);
            }
        }

        virtual void visit(IWriteVisitor* visitor_) const override
        {
            uint64_t size = m_ptr->size();
            auto& visitor = *visitor_;
            const auto trait = visitor.traits();

            for (auto itr = m_ptr->begin(); itr != m_ptr->end(); ++itr)
            {
                if (trait.supports_named_access)
                {
                    visitor(&itr->second, itr->first);
                }
                else
                {
                    std::string key = itr->first;
                    visitor(&key);
                    visitor(&itr->second);
                }
            }
        }

        virtual TypeInfo keyType() const override { return TypeInfo(typeid(std::string)); }
        virtual TypeInfo valueType() const override { return TypeInfo(typeid(V)); }
        virtual TypeInfo type() const { return TypeInfo(typeid(std::map<std::string, V>)); }
        virtual bool isContinuous() const override { return false; }
        virtual bool podValues() const override { return std::is_pod<V>::value; }
        virtual bool podKeys() const override { return std::is_pod<std::string>::value; }
        virtual size_t numKeys() const override { return m_ptr->size(); }
        virtual size_t numValues() const override { return m_ptr->size(); }
        virtual void setKeys(const size_t num) override { num_to_read = num; }
        virtual void setValues(const size_t) override {}
    };

    template <class V>
    struct TTraits<const std::map<std::string, V>, void> : public IContainerTraits
    {
        using base = IContainerTraits;
        const std::map<std::string, V>* m_ptr;
        size_t num_to_read = 0;
        TTraits(const std::map<std::string, V>* ptr) : m_ptr(ptr) {}

        virtual void visit(IReadVisitor*) override {}

        virtual void visit(IWriteVisitor* visitor_) const override
        {
            uint64_t size = m_ptr->size();
            auto& visitor = *visitor_;
            const auto trait = visitor.traits();

            for (auto itr = m_ptr->begin(); itr != m_ptr->end(); ++itr)
            {
                if (trait.supports_named_access)
                {
                    visitor(&itr->second, itr->first);
                }
                else
                {
                    std::string key = itr->first;
                    visitor(&key);
                    visitor(&itr->second);
                }
            }
        }

        virtual TypeInfo keyType() const override { return TypeInfo(typeid(std::string)); }
        virtual TypeInfo valueType() const override { return TypeInfo(typeid(V)); }
        virtual TypeInfo type() const { return TypeInfo(typeid(std::map<std::string, V>)); }
        virtual bool isContinuous() const override { return false; }
        virtual bool podValues() const override { return std::is_pod<V>::value; }
        virtual bool podKeys() const override { return std::is_pod<std::string>::value; }
        virtual size_t numKeys() const override { return m_ptr->size(); }
        virtual size_t numValues() const override { return m_ptr->size(); }
        virtual void setKeys(const size_t num) override { num_to_read = num; }
        virtual void setValues(const size_t) override {}
    };
}
