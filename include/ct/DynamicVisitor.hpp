#pragma once
#include "IDynamicVisitor.hpp"
#include <unordered_map>
namespace ct
{
    class DynamicVisitor : public IDynamicVisitor
    {
      public:
        virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "");

      protected:
        virtual std::unique_ptr<IDataContainer>& accessCache(const std::string& name, const uint64_t id = 0) override;
        virtual void* getPointer(const TypeInfo type, const uint64_t id) override;
        virtual void setSerializedPointer(const TypeInfo type, const uint64_t id, void* ptr) override;

      private:
        std::unordered_map<TypeInfo, std::unordered_map<uint64_t, void*>> m_serialized_pointers;
        std::unordered_map<std::string, std::unordered_map<uint64_t, std::unique_ptr<IDataContainer>>> m_cache;
    };
}
