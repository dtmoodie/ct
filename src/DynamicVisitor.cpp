#include "ct/DynamicVisitor.hpp"

namespace ct
{
    IDataContainer::~IDataContainer() {}
    IDynamicVisitor::~IDynamicVisitor() {}

    IDynamicVisitor& DynamicVisitor::operator()(IStructTraits* val, const std::string& name)
    {
        const auto traits = this->traits();
        // writing data out,
        void* ptr = val->ptr();
        const uint64_t id = uint64_t(ptr);
        m_serialized_pointers[val->type()][id] = ptr;

        val->visit(this);

        if (traits.reader)
        {
            // reading data in
            // void* ptr = val->ptr();
        }
        else
        {
            // writing data out
        }
        return *this;
    }

    std::unique_ptr<IDataContainer>& DynamicVisitor::accessCache(const std::string& name, const uint64_t id)
    {
        return m_cache[name][id];
    }

    void DynamicVisitor::setSerializedPointer(const TypeInfo type, const uint64_t id, void* ptr)
    {
        m_serialized_pointers[type][id] = ptr;
    }

    void* DynamicVisitor::getPointer(const TypeInfo type, const uint64_t id)
    {
        auto itr1 = m_serialized_pointers.find(type);
        if (itr1 != m_serialized_pointers.end())
        {
            auto itr2 = itr1->second.find(id);
            if (itr2 != itr1->second.end())
            {
                return itr2->second;
            }
        }

        return nullptr;
    }
}
