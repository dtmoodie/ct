#include "../reflect/Data.hpp"
#include "../reflect/Reflect.hpp"
#include "../reflect/common.hpp"

#include "BinaryReader.hpp"
#include "BinaryWriter.hpp"
#include "JSONPrinter.hpp"

#include <ct/TypeTraits.hpp>
#include <ct/reflect.hpp>
#include <ct/reflect/compare-inl.hpp>
#include <ct/visitor_traits/map.hpp>
#include <ct/visitor_traits/memory.hpp>
#include <ct/visitor_traits/vector.hpp>

#include <ct/reflect/cerealize.hpp>
#include <ct/reflect/compare.hpp>
#include <ct/reflect/print.hpp>

#include <ct/reflect/compare-container-inl.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include <cassert>
#include <fstream>
#include <map>
#include <type_traits>

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
        virtual void visit(ct::IReadVisitor* visitor) override
        {
            (*visitor) (&m_vec->x, "x") (&m_vec->y, "y")(&m_vec->z, "z");
        }

        virtual void visit(ct::IWriteVisitor* visitor) const override
        {
            (*visitor) (&m_vec->x, "x") (&m_vec->y, "y")(&m_vec->z, "z");
        }
        virtual const void* ptr() const { return m_vec; }
        virtual void* ptr() { return m_vec; }
    };

    template <class T, index_t I>
    void visitValue(IReadVisitor& visitor, T& obj)
    {
        auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});
        using RefType = typename ReferenceType<typename decltype(accessor)::SetType>::Type;
        visitor(&static_cast<RefType>(accessor.set(obj)), Reflect<T>::getName(ct::Indexer<I>{}));
    }

    template <class T, index_t I>
    void visitValue(IWriteVisitor& visitor, const T& obj)
    {
        auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});
        using RefType = typename ReferenceType<typename decltype(accessor)::GetType>::ConstType;
        RefType ref = static_cast<RefType>(accessor.get(obj));
        visitor(&ref, Reflect<T>::getName(ct::Indexer<I>{}));
    }

    template <class T>
    void visitHelper(IReadVisitor& visitor, T& obj, const Indexer<0>)
    {
        visitValue<T, 0>(visitor, obj);
    }

    template <class T, index_t I>
    void visitHelper(IReadVisitor& visitor, T& obj, const Indexer<I>)
    {
        visitHelper(visitor, obj, Indexer<I - 1>{});
        visitValue<T, I>(visitor, obj);
    }

    template <class T>
    void visitHelper(IWriteVisitor& visitor, const T& obj, const Indexer<0>)
    {
        visitValue<T, 0>(visitor, obj);
    }

    template <class T, index_t I>
    void visitHelper(IWriteVisitor& visitor, const T& obj, const Indexer<I>)
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

        virtual void visit(ct::IReadVisitor* visitor) override { visitHelper(*visitor, *m_ptr, Reflect<T>::end()); }
        virtual void visit(ct::IWriteVisitor* visitor) const override {}

        virtual bool isPrimitiveType() const override { return false; }

        virtual size_t size() const override { return sizeof(T); }
        virtual TypeInfo type() const { return TypeInfo(typeid(T)); }
        virtual bool triviallySerializable() const override { return std::is_pod<T>::value; }
        virtual const void* ptr() const override { return m_ptr; }
        virtual void* ptr() override { return m_ptr; }
    };

    template <class T>
    struct TTraits<const T, ct::enable_if_reflected<T>> : public IStructTraits
    {
        using base = IStructTraits;
        const T* m_ptr;
        TTraits(const T* ptr) : m_ptr(ptr) {}

        virtual void visit(ct::IReadVisitor* visitor) override {}
        virtual void visit(ct::IWriteVisitor* visitor) const override
        {
            visitHelper(*visitor, *m_ptr, Reflect<T>::end());
        }

        virtual bool isPrimitiveType() const override { return false; }

        virtual size_t size() const override { return sizeof(T); }
        virtual TypeInfo type() const { return TypeInfo(typeid(T)); }
        virtual bool triviallySerializable() const override { return std::is_pod<T>::value; }
        virtual const void* ptr() const override { return m_ptr; }
        virtual void* ptr() override { return nullptr; }
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
}

struct TestBinary
{
    template <class T>
    void test(const T& data)
    {
        {
            std::ofstream ofs("test.bin", std::ios::binary | std::ios::out);
            ct::BinaryWriter bar(ofs);
            ct::IWriteVisitor& visitor = bar;
            T tmp = data;
            visitor(&tmp, "value0");
        }
        {
            std::ifstream ifs("test.bin", std::ios::binary | std::ios::in);
            ct::BinaryReader bar(ifs);
            ct::IReadVisitor& visitor = bar;

            T tmp;
            visitor(&tmp, "value0");
            if (!ct::compare(tmp, data, DebugEqual()))
            {
                std::cout << "Failed to serialize " << ct::Reflect<T>::getName() << " correctly";
                throw std::runtime_error("Serialization failed");
            }
        }
    }

    template <class T>
    void test(const std::shared_ptr<T>& data)
    {
        {
            std::ofstream ofs("test.bin", std::ios::binary | std::ios::out);
            ct::BinaryWriter bar(ofs);
            ct::IWriteVisitor& visitor = bar;
            std::shared_ptr<T> tmp = data;
            visitor(&tmp, "value0");
            visitor(&tmp, "value1");
        }
        {
            std::ifstream ifs("test.bin", std::ios::binary | std::ios::in);
            ct::BinaryReader bar(ifs);
            ct::IReadVisitor& visitor = bar;

            std::shared_ptr<T> tmp;
            std::shared_ptr<T> tmp2;
            visitor(&tmp, "value0");
            visitor(&tmp2, "value1");
            if (!ct::compare(tmp, data, DebugEqual()))
            {
                std::cout << "Failed to serialize " << ct::Reflect<T>::getName() << " correctly";
                throw std::runtime_error("Serialization failed");
            }
            if (tmp.get() != tmp2.get())
            {
                std::cout << "Failed to reshare ownership of two smart pointers" << std::endl;
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
            ct::IWriteVisitor& visitor = printer;
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

    template <class T>
    void test(const std::shared_ptr<T>& data)
    {
        std::shared_ptr<T> tmp = data;
        std::stringstream ss;
        {
            ct::JSONWriter printer(ss);
            ct::IWriteVisitor& visitor = printer;
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
    }
};

int main()
{
    {
        using Accessor_t = decltype(ct::Reflect<WeirdWeakOwnerShip>::getAccessor(ct::Indexer<1>{}));
        using Get_t = Accessor_t::RetType;
        static_assert(std::is_same<const std::vector<PointerOwner>&, Get_t>::value, "asdf");
    }
    TestBinary tester;
    testTypes(tester);

    TestJson test_json;
    testTypes(test_json);

    std::cout << std::endl;
    std::shared_ptr<ReflectedStruct> shared_ptr = std::make_shared<ReflectedStruct>();

    {
        ct::JSONWriter writer(std::cout);
        ct::IWriteVisitor& visitor = writer;
        visitor(&shared_ptr);
        visitor(&shared_ptr);
    }
    std::cout << std::endl;
}
