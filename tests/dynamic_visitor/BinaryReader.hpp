#pragma once
#include "ct/DynamicVisitor.hpp"
#include <iostream>
#include <unordered_map>
namespace ct
{
    class BinaryReader : public DynamicVisitor
    {
      public:
        BinaryReader(std::istream& in);
        virtual ~BinaryReader() override;

        virtual IDynamicVisitor& operator()(char* ptr, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(int8_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(uint8_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(int16_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(uint16_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(int32_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(uint32_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(int64_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(uint64_t* ptr, const std::string& name, const size_t cnt) override;

        virtual IDynamicVisitor& operator()(float* val, const std::string& name, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(double*, const std::string&, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(void*, const std::string&, const size_t cnt) override;
        virtual IDynamicVisitor& operator()(IStructTraits* val, const std::string& name = "") override;
        virtual IDynamicVisitor& operator()(IContainerTraits* val, const std::string& name = "") override;

        virtual VisitorTraits traits() const override;

      private:
        template <class T>
        IDynamicVisitor& readBinary(T* ptr, const size_t cnt = 1);

        std::istream& m_is;
    };
}
