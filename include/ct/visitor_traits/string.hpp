#pragma once
#include "../DynamicVisitor.hpp"
#include <string>

namespace ct
{
    template <>
    struct TTraits<std::string, void> : public IContainerTraits
    {
        using base = IContainerTraits;

        TTraits(std::string* ptr);
        virtual void visit(ct::IReadVisitor* visitor) override;
        virtual void visit(ct::IWriteVisitor* visitor) const override;
        virtual TypeInfo keyType() const override;
        virtual TypeInfo valueType() const override;

        virtual TypeInfo type() const override;
        virtual bool isContinuous() const override;
        virtual bool podValues() const override;
        virtual bool podKeys() const override;
        virtual size_t numKeys() const override;
        virtual size_t numValues() const override;

        virtual void setKeys(const size_t) override;
        virtual void setValues(const size_t num) override;

      private:
        std::string* m_ptr;
        size_t num_to_read = 0;
    };

    template <>
    struct TTraits<const std::string, void> : public IContainerTraits
    {
        using base = IContainerTraits;

        TTraits(const std::string* ptr);
        virtual void visit(ct::IReadVisitor* visitor) override;
        virtual void visit(ct::IWriteVisitor* visitor) const override;
        virtual TypeInfo keyType() const override;
        virtual TypeInfo valueType() const override;

        virtual TypeInfo type() const override;
        virtual bool isContinuous() const override;
        virtual bool podValues() const override;
        virtual bool podKeys() const override;
        virtual size_t numKeys() const override;
        virtual size_t numValues() const override;

        virtual void setKeys(const size_t) override;
        virtual void setValues(const size_t num) override;

      private:
        const std::string* m_ptr;
        size_t num_to_read = 0;
    };
}
