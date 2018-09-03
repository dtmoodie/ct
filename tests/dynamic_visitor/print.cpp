#include "../reflect/Data.hpp"
#include "../reflect/Reflect.hpp"
#include "../reflect/common.hpp"

#include "BinaryReader.hpp"
#include "BinaryWriter.hpp"
#include "JSONPrinter.hpp"

#include <ct/TypeTraits.hpp>
#include <ct/reflect.hpp>
#include <ct/reflect/compare-inl.hpp>

#include <ct/reflect/cerealize.hpp>
#include <ct/reflect/compare.hpp>
#include <ct/reflect/print.hpp>

#include <ct/reflect/compare-container-inl.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>

#include <cassert>
#include <fstream>
#include <map>

namespace cereal
{
//! Saving for std::map<std::string, std::string> for text based archives
// Note that this shows off some internal cereal traits such as EnableIf,
// which will only allow this template to be instantiated if its predicates
// are true
template <class Archive,
          class T,
          class C,
          class A,
          traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline void save(Archive& ar, std::map<std::string, T, C, A> const& map)
{
    for (const auto& i : map)
        ar(cereal::make_nvp(i.first, i.second));
}

//! Loading for std::map<std::string, std::string> for text based archives
template <class Archive,
          class T,
          class C,
          class A,
          traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline void load(Archive& ar, std::map<std::string, T, C, A>& map)
{
    map.clear();

    auto hint = map.begin();
    while (true)
    {
        const auto namePtr = ar.getNodeName();

        if (!namePtr)
            break;

        std::string key = namePtr;
        T value;
        ar(value);
        hint = map.emplace_hint(hint, std::move(key), std::move(value));
    }
}
} // namespace cereal

struct Vec
{
    float x, y, z;
};
namespace ct
{
template <>
struct TTraits<Vec, void> : IStructTraits
{
    Vec* m_vec;
    using base = IStructTraits;
    TTraits(Vec* vec) : m_vec(vec) {}

    virtual size_t size() const { return sizeof(Vec); }
    virtual bool isPrimitiveType() const override { return false; }
    virtual bool triviallySerializable() const { return std::is_pod<Vec>::value; }
    virtual void visit(ct::IDynamicVisitor* visitor) override
    {
        (*visitor) (&m_vec->x, "x") (&m_vec->y, "y")(&m_vec->z, "z");
    }
    virtual const void* ptr() const { return m_vec; }
    virtual void* ptr() { return m_vec; }
};

template <class T, index_t I>
void visitValue(IDynamicVisitor& visitor, T& obj)
{
    auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});
    using RefType = typename ReferenceType<typename decltype(accessor)::SetType>::Type;
    visitor(&static_cast<RefType>(accessor.set(obj)), Reflect<T>::getName(ct::Indexer<I>{}));
}

template <class T>
void visitHelper(IDynamicVisitor& visitor, T& obj, const Indexer<0>)
{
    visitValue<T, 0>(visitor, obj);
}

template <class T, index_t I>
void visitHelper(IDynamicVisitor& visitor, T& obj, const Indexer<I>)
{
    visitHelper(visitor, obj, Indexer<I - 1>{});
    visitValue<T, I>(visitor, obj);
}

template <class T>
struct TTraits<T, ct::enable_if_reflected<T>> : public IStructTraits
{
    using base = IStructTraits;
    T* m_ptr;
    TTraits(T* ptr) : m_ptr(ptr) {}

    virtual void visit(ct::IDynamicVisitor* visitor) override { visitHelper(*visitor, *m_ptr, Reflect<T>::end()); }

    virtual bool isPrimitiveType() const override { return false; }

    virtual size_t size() const override { return sizeof(T); }

    virtual bool triviallySerializable() const override { return std::is_pod<T>::value; }
    virtual const void* ptr() const override { return m_ptr; }
    virtual void* ptr() override { return m_ptr; }
};

template <>
struct TTraits<std::string, void> : public IContainerTraits
{
    using base = IContainerTraits;
    std::string* m_ptr;
    size_t num_to_read = 0;
    TTraits(std::string* ptr) : m_ptr(ptr) {}
    virtual void visit(ct::IDynamicVisitor* visitor) override { (*visitor)(&(*m_ptr)[0], "", m_ptr->size()); }
    virtual TypeInfo keyType() const override { return TypeInfo(typeid(void)); }
    virtual TypeInfo valueType() const override { return TypeInfo(typeid(char)); }

    virtual TypeInfo containerType() const { return TypeInfo(typeid(std::string)); }
    virtual bool isContinuous() const override { return false; }
    virtual bool podValues() const override { return true; }
    virtual bool podKeys() const override { return false; }
    virtual size_t numKeys() const override { return 0; }
    virtual size_t numValues() const override { return m_ptr->size(); }

    virtual void setKeys(const size_t) override {}
    virtual void setValues(const size_t num) override { m_ptr->resize(num); }
};

template <class T1, class T2>
struct TTraits<std::pair<T1, T2>, void> : public IStructTraits
{
    using base = IStructTraits;
    std::pair<T1, T2>* m_ptr;
    TTraits(std::pair<T1, T2>* ptr) : m_ptr(ptr) {}

    virtual void visit(ct::IDynamicVisitor* visitor) override
    {
        (*visitor)(&m_ptr->first, "first");
        (*visitor)(&m_ptr->second, "second");
    }
    virtual size_t size() const { return sizeof(std::pair<T1, T2>); }
    virtual bool triviallySerializable() const { return std::is_pod<T1>::value && std::is_pod<T2>::value; }
    virtual bool isPrimitiveType() const { return false; }
    virtual const void* ptr() const { return m_ptr; }
    virtual void* ptr() { return m_ptr; }
};

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

    virtual void visit(ct::IDynamicVisitor* visitor) override
    {
        (*visitor)(&m_ptr->key, "key");
        (*visitor)(&m_ptr->value, "value");
    }
    virtual size_t size() const { return sizeof(KVP<T1, T2>); }
    virtual bool triviallySerializable() const { return std::is_pod<T1>::value && std::is_pod<T2>::value; }
    virtual bool isPrimitiveType() const { return false; }
    virtual const void* ptr() const { return m_ptr; }
    virtual void* ptr() { return m_ptr; }
};

template <class K, class V>
struct TTraits<std::map<K, V>, void> : public IContainerTraits
{
    using base = IContainerTraits;
    std::map<K, V>* m_ptr;
    size_t num_to_read = 0;
    TTraits(std::map<K, V>* ptr) : m_ptr(ptr) {}

    virtual void visit(IDynamicVisitor* visitor_) override
    {
        uint64_t size = m_ptr->size();
        IDynamicVisitor& visitor = *visitor_;
        const auto trait = visitor.traits();
        if (!trait.reader)
        {
            for (auto itr = m_ptr->begin(); itr != m_ptr->end(); ++itr)
            {
                KVP<K, V> pair;
                pair = *itr;
                visitor(&pair);
                // visitor(&itr->second, "value");
            }
        }
        else
        {
            for (size_t i = 0; i < num_to_read; ++i)
            {
                K key;
                V val;
                visitor(&key, "key");
                visitor(&val, "value");
                (*m_ptr)[std::move(key)] = std::move(val);
            }
        }
    }

    virtual TypeInfo keyType() const override { return TypeInfo(typeid(K)); }
    virtual TypeInfo valueType() const override { return TypeInfo(typeid(V)); }
    virtual TypeInfo containerType() const { return TypeInfo(typeid(std::map<K, V>)); }
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

    virtual void visit(IDynamicVisitor* visitor_) override
    {
        uint64_t size = m_ptr->size();
        IDynamicVisitor& visitor = *visitor_;
        const auto trait = visitor.traits();
        if (!trait.reader)
        {
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
        else
        {
            for (size_t i = 0; i < num_to_read; ++i)
            {
                std::string key;
                visitor(&key);
                V val;
                visitor(&val);
                (*m_ptr)[std::move(key)] = std::move(val);
            }
        }
    }

    virtual TypeInfo keyType() const override { return TypeInfo(typeid(std::string)); }
    virtual TypeInfo valueType() const override { return TypeInfo(typeid(V)); }
    virtual TypeInfo containerType() const { return TypeInfo(typeid(std::map<std::string, V>)); }
    virtual bool isContinuous() const override { return false; }
    virtual bool podValues() const override { return std::is_pod<V>::value; }
    virtual bool podKeys() const override { return std::is_pod<std::string>::value; }
    virtual size_t numKeys() const override { return m_ptr->size(); }
    virtual size_t numValues() const override { return m_ptr->size(); }
    virtual void setKeys(const size_t num) override { num_to_read = num; }
    virtual void setValues(const size_t) override {}
};
}

struct TestBinary
{
    template <class T>
    void test(const T& data)
    {
        {
            std::ofstream ofs("test.bin", std::ios::binary | std::ios::out);
            ct::BinaryWriter bar(ofs);
            ct::IDynamicVisitor& visitor = bar;
            T tmp = data;
            visitor(&tmp, "value0");
        }
        {
            std::ifstream ifs("test.bin", std::ios::binary | std::ios::in);
            ct::BinaryReader bar(ifs);
            ct::IDynamicVisitor& visitor = bar;

            T tmp;
            visitor(&tmp, "value0");
            if (!ct::compare(tmp, data, DebugEqual()))
            {
                std::cout << "Failed to serialize " << ct::Reflect<T>::getName() << " correctly";
                throw std::runtime_error("Serialization failed");
            }
        }
    }
};

struct TestJson
{
    template <class T>
    void test(const T& data)
    {
        T tmp = data;
        std::stringstream ss;
        {
            ct::JSONWriter printer(ss);
            ct::IDynamicVisitor& visitor = printer;
            visitor(&tmp);
        }
        ss.seekg(std::ios::beg);
        std::cout << "------------------------------\nDynamic\n";
        std::cout << ss.str() << std::endl;
        std::cout << "------------------------------\nStatic\n";
        {
            cereal::JSONOutputArchive ar(std::cout);
            ar(tmp);
        }
        cereal::JSONInputArchive ar(ss);
        T tmp1;
        ar(tmp1);
        if (!ct::compare(tmp, tmp1, DebugEqual()))
        {
            std::cout << "Failed to load from json " << ct::Reflect<T>::getName() << " correctly";
            throw std::runtime_error("Json serialization failed");
        }
    }
};

int main()
{
    TestBinary tester;
    testTypes(tester);

    TestJson test_json;
    testTypes(test_json);
}
