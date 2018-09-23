#pragma once
#include "ct/DynamicVisitor.hpp"
#include <iostream>
#include <unordered_map>

namespace ct
{
    class BinaryWriter : public WriteCache
    {
      public:
        BinaryWriter(std::ostream& in);
        virtual ~BinaryWriter() override;

        virtual IWriteVisitor& operator()(const char* ptr, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const int8_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const uint8_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const int16_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const uint16_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const int32_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const uint32_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const int64_t* ptr, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const uint64_t* ptr, const std::string& name, const size_t cnt) override;

        virtual IWriteVisitor& operator()(const float* val, const std::string& name, const size_t cnt) override;
        virtual IWriteVisitor& operator()(const double*, const std::string&, const size_t) override;
        virtual IWriteVisitor& operator()(const void*, const std::string&, const size_t) override;
        virtual IWriteVisitor& operator()(const IStructTraits* val, const std::string& name = "") override;
        virtual IWriteVisitor& operator()(const IContainerTraits* val, const std::string& name = "") override;

        virtual VisitorTraits traits() const override;

      private:
        template <class T>
        IWriteVisitor& writeBinary(const T* ptr, const std::string& name = "", const size_t cnt = 1);
        std::ostream& m_os;
    };
}
