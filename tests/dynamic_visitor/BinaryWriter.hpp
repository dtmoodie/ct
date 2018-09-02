#pragma once
#include "ct/DynamicVisitor.hpp"
#include <iostream>
#include <unordered_map>

namespace ct
{
class BinaryWriter: public IDynamicVisitor
{
public:
    BinaryWriter(std::ostream& in);
    virtual ~BinaryWriter() override;

    virtual IDynamicVisitor& operator()(char* ptr, const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(int8_t* ptr,         const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(uint8_t* ptr,        const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(int16_t* ptr,        const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(uint16_t* ptr,       const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(int32_t* ptr,        const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(uint32_t* ptr,       const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(int64_t* ptr,        const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(uint64_t* ptr,       const std::string& name, const size_t cnt) override;

    virtual IDynamicVisitor& operator()(float* val, const std::string& name, const size_t cnt) override;
    virtual IDynamicVisitor& operator()(double* ,         const std::string& , const size_t ) override;
    virtual IDynamicVisitor& operator()(void* ,        const std::string& , const size_t ) override;
    virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "") override;
    virtual std::unique_ptr<IDataContainer>& accessCache(const std::string& name) override;
    virtual bool reading() const override;
    virtual bool isTextVisitor() const;
private:
    virtual IDynamicVisitor &startContainer(IContainerTraits &,
        const std::string &name) override;
    virtual IDynamicVisitor &endContainer() override;


    template<class T>
    IDynamicVisitor& writeBinary(T* ptr, const std::string& name = "", const size_t cnt = 1);

    std::unordered_map<std::string, std::unique_ptr<ct::IDataContainer>> m_cache;
    std::ostream& m_os;
};
}
