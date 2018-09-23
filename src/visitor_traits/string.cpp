#include "ct/visitor_traits/string.hpp"

namespace ct
{

    TTraits<std::string, void>::TTraits(std::string* ptr) : m_ptr(ptr) {}

    void TTraits<std::string, void>::visit(ct::IReadVisitor* visitor) { (*visitor)(&(*m_ptr)[0], "", m_ptr->size()); }

    void TTraits<std::string, void>::visit(ct::IWriteVisitor* visitor) const
    {
        (*visitor)(&(*m_ptr)[0], "", m_ptr->size());
    }

    TypeInfo TTraits<std::string, void>::keyType() const { return TypeInfo(typeid(void)); }

    TypeInfo TTraits<std::string, void>::valueType() const { return TypeInfo(typeid(char)); }

    TypeInfo TTraits<std::string, void>::type() const { return TypeInfo(typeid(std::string)); }

    bool TTraits<std::string, void>::isContinuous() const { return false; }

    bool TTraits<std::string, void>::podValues() const { return true; }

    bool TTraits<std::string, void>::podKeys() const { return false; }

    size_t TTraits<std::string, void>::numKeys() const { return 0; }

    size_t TTraits<std::string, void>::numValues() const { return m_ptr->size(); }

    void TTraits<std::string, void>::setKeys(const size_t) {}

    void TTraits<std::string, void>::setValues(const size_t num) { m_ptr->resize(num); }

    TTraits<const std::string, void>::TTraits(const std::string* ptr) : m_ptr(ptr) {}

    void TTraits<const std::string, void>::visit(ct::IReadVisitor*) {}

    void TTraits<const std::string, void>::visit(ct::IWriteVisitor* visitor) const
    {
        (*visitor)(&(*m_ptr)[0], "", m_ptr->size());
    }

    TypeInfo TTraits<const std::string, void>::keyType() const { return TypeInfo(typeid(void)); }

    TypeInfo TTraits<const std::string, void>::valueType() const { return TypeInfo(typeid(char)); }

    TypeInfo TTraits<const std::string, void>::type() const { return TypeInfo(typeid(std::string)); }

    bool TTraits<const std::string, void>::isContinuous() const { return false; }

    bool TTraits<const std::string, void>::podValues() const { return true; }

    bool TTraits<const std::string, void>::podKeys() const { return false; }

    size_t TTraits<const std::string, void>::numKeys() const { return 0; }

    size_t TTraits<const std::string, void>::numValues() const { return m_ptr->size(); }

    void TTraits<const std::string, void>::setKeys(const size_t) {}

    void TTraits<const std::string, void>::setValues(const size_t) {}
}
