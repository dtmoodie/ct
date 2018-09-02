#include "../reflect/Data.hpp"
#include "../reflect/Reflect.hpp"
#include "JSONPrinter.hpp"
#include "BinaryWriter.hpp"
#include "BinaryReader.hpp"
#include <ct/TypeTraits.hpp>
#include <ct/reflect.hpp>
#include <fstream>
#include <map>
struct Vec {
  float x, y, z;
};
namespace ct {
template <> struct TTraits<Vec, void> : IStructTraits {
  Vec *m_vec;
  using base = IStructTraits;
  TTraits(Vec *vec) : m_vec(vec) {}

  virtual size_t size() const { return sizeof(Vec); }
  virtual bool isPrimitiveType() const override { return false; }
  virtual bool triviallySerializable() const { return std::is_pod<Vec>::value; }
  virtual void visit(ct::IDynamicVisitor *visitor) override {
    (*visitor) (&m_vec->x, "x") (&m_vec->y, "y")(&m_vec->z, "z");
  }
  virtual const void *ptr() const { return m_vec; }
  virtual void *ptr() { return m_vec; }
};

template <class T, index_t I>
void visitValue(IDynamicVisitor &visitor, T &obj) {
  auto accessor = Reflect<T>::getAccessor(ct::Indexer<I>{});
  using RefType =
      typename ReferenceType<typename decltype(accessor)::SetType>::Type;
  visitor(&static_cast<RefType>(accessor.set(obj)),
          Reflect<T>::getName(ct::Indexer<I>{}));
}

template <class T>
void visitHelper(IDynamicVisitor &visitor, T &obj, const Indexer<0>) {
  visitValue<T, 0>(visitor, obj);
}

template <class T, index_t I>
void visitHelper(IDynamicVisitor &visitor, T &obj, const Indexer<I>) {
  visitHelper(visitor, obj, Indexer<I - 1>{});
  visitValue<T, I>(visitor, obj);
}

template <class T>
struct TTraits<T, ct::enable_if_reflected<T>> : public IStructTraits {
  using base = IStructTraits;
  T *m_ptr;
  TTraits(T *ptr) : m_ptr(ptr) {}

  virtual void visit(ct::IDynamicVisitor *visitor) override {
    visitHelper(*visitor, *m_ptr, Reflect<T>::end());
  }

  virtual bool isPrimitiveType() const override { return false; }

  virtual size_t size() const override { return sizeof(T); }

  virtual bool triviallySerializable() const override {
    return std::is_pod<T>::value;
  }
  virtual const void *ptr() const override { return m_ptr; }
  virtual void *ptr() override { return m_ptr; }
};

template <> struct TTraits<std::string, void> : public IContainerTraits {
  using base = IContainerTraits;
  std::string *m_ptr;
  size_t num_to_read = 0;
  TTraits(std::string *ptr) : m_ptr(ptr) {}
  virtual void visit(ct::IDynamicVisitor *visitor) override {
    (*visitor)(&(*m_ptr)[0], "", m_ptr->size());
  }
  virtual TypeInfo keyType() const override { return TypeInfo(typeid(void)); }
  virtual TypeInfo valueType() const override { return TypeInfo(typeid(char)); }
  virtual bool isContinuous() const override { return false; }
  virtual bool podValues() const override { return true; }
  virtual bool podKeys() const override { return false; }
  virtual size_t numKeys() const override { return 0; }
  virtual size_t numValues() const override { return m_ptr->size(); }

  virtual void setKeys(const size_t) override { 
      
  }
  virtual void setValues(const size_t num) override {
      m_ptr->resize(num);
  }
};

template <class K, class V>
struct TTraits<std::map<K, V>, void> : public IContainerTraits {
  using base = IContainerTraits;
  std::map<K, V> *m_ptr;
  size_t num_to_read = 0;
  TTraits(std::map<K, V> *ptr) : m_ptr(ptr) {}

  virtual void visit(IDynamicVisitor *visitor_) override {
    uint64_t size = m_ptr->size();
    IDynamicVisitor &visitor = *visitor_;
    if (visitor.reading()) {
      for (auto itr = m_ptr->begin(); itr != m_ptr->end(); ++itr) {
        K key = itr->first;
        visitor(&key, "key");
        visitor(&itr->second, "value");
      }
    } else {
    }
  }

  virtual TypeInfo keyType() const override { return TypeInfo(typeid(K)); }
  virtual TypeInfo valueType() const override { return TypeInfo(typeid(V)); }
  virtual bool isContinuous() const override { return false; }
  virtual bool podValues() const override { return std::is_pod<V>::value; }
  virtual bool podKeys() const override { return std::is_pod<K>::value; }
  virtual size_t numKeys() const override { return m_ptr->size(); }
  virtual size_t numValues() const override { return m_ptr->size(); }

  virtual void setKeys(const size_t num) override { num_to_read = num; }
  virtual void setValues(const size_t) override {}
};

template <class V>
struct TTraits<std::map<std::string, V>, void> : public IContainerTraits {
  using base = IContainerTraits;
  std::map<std::string, V> *m_ptr;
  size_t num_to_read = 0;
  TTraits(std::map<std::string, V> *ptr) : m_ptr(ptr) {}

  virtual void visit(IDynamicVisitor *visitor_) override {
    uint64_t size = m_ptr->size();
    IDynamicVisitor &visitor = *visitor_;
    if (visitor.reading()) {
        const bool is_text = visitor.isTextVisitor();
      for (auto itr = m_ptr->begin(); itr != m_ptr->end(); ++itr) {
          if (is_text)
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
    } else {
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

  virtual TypeInfo keyType() const override {
    return TypeInfo(typeid(std::string));
  }
  virtual TypeInfo valueType() const override { return TypeInfo(typeid(V)); }
  virtual bool isContinuous() const override { return false; }
  virtual bool podValues() const override { return std::is_pod<V>::value; }
  virtual bool podKeys() const override {
    return std::is_pod<std::string>::value;
  }
  virtual size_t numKeys() const override { return m_ptr->size(); }
  virtual size_t numValues() const override { return m_ptr->size(); }
  virtual void setKeys(const size_t num) override { num_to_read = num; }
  virtual void setValues(const size_t) override {}
};
}
int main() {
  ct::TTraits<std::string, void> trait(nullptr);
  {
    ct::JSONPrinter visitor_impl(std::cout, true);
    ct::IDynamicVisitor &visitor = visitor_impl;
    Vec vec{0, 1, 2};
    visitor(&vec);
    ReflectedStruct s1;
    visitor(&s1);
    ReflectedStruct s2[3];
    visitor(s2, "array", 3);

    Composite c1;
    visitor(&c1, "composite");
    {
      std::vector<TestC> vec{{0, 1, 2}, {2, 3, 4}, {4, 5, 6}};
      visitor(&vec, "vec");
    }
    {
      PrivateMutableAccess obj;
      visitor(&obj, "pma");
    }
    {
      PrivateGetAndSet obj;
      obj.setX(5);
      visitor(&obj);
    }
    {
      InternallyReflected obj;
      visitor(&obj);
    }
    {
      Inherited obj;
      visitor(&obj);
    }
    {
      std::map<std::string, Vec> map;
      map["first"] = {0, 1, 2};
      map["second"] = {2, 3, 4};
      visitor(&map, "map");
    }
    {
      std::map<int, Vec> map;
      map[0] = {0, 1, 2};
      map[1] = {2, 3, 4};
      visitor(&map, "int_map");
    }
  }

  std::cout << std::endl;

  {
      std::ofstream ofs("test.bin", std::ios::binary | std::ios::out);
      ct::BinaryWriter bar(ofs);
      ct::IDynamicVisitor &visitor = bar;
      int val = 0;
      float pi = 3.14;
      visitor(&val, "value0");
      visitor(&pi, "pi");
      std::vector<float> vec{ 0.0f, 0.1f, 0.2f, 0.3f };
      visitor(&vec);
      std::map<std::string, Vec> vecmap;
      vecmap["asdf"] = { 0,1,2 };
      vecmap["sdfg"] = { 0,1,4 };
      visitor(&vecmap);
      ofs.close();
  }
  {
      std::ifstream ifs("test.bin", std::ios::binary | std::ios::in);
      if (ifs)
      {
          ifs.seekg(std::ios::beg);
          auto start = ifs.tellg();
          ifs.seekg(std::ios::end);
          auto end = ifs.tellg();
          std::cout << "size: " << end - start << std::endl;
          ifs.seekg(std::ios::beg);
          
          ct::BinaryReader bar(ifs);
          ct::IDynamicVisitor &visitor = bar;
          int val = 1;
          float pi = 0;
          visitor(&val, "value0");
          visitor(&pi, "pi");
          std::vector<float> vec;
          visitor(&vec);
          std::map<std::string, Vec> vecmap;
          visitor(&vecmap);
          std::cout << val << std::endl;
          std::cout << pi << std::endl;
          
            
          
      }
  }
}
