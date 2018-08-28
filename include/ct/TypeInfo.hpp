#pragma once
#include <string>
#include <typeinfo>

namespace ct
{
    class TypeInfo
    {
      public:
        template<class T>
        static TypeInfo create()
        {
            return TypeInfo(typeid(T));
        }
        inline TypeInfo();
        inline TypeInfo(const std::type_info&);
        template <class T>
        inline TypeInfo(const T& obj);

        inline const std::type_info& get() const;
        inline bool before(const TypeInfo& rhs) const;
        inline std::string name() const;

        template <class T>
        inline bool isType() const;

        inline bool operator==(const std::type_info& rhs) const;
        inline bool operator!=(const std::type_info& rhs) const;

      private:
        const std::type_info* pInfo_;
    };

    template <class T>
    TypeInfo::TypeInfo(const T& obj) : TypeInfo(typeid(obj))
    {
    }

    template <class T>
    bool TypeInfo::isType() const
    {
        return *this == typeid(T);
    }

    TypeInfo::TypeInfo()
    {
        class Void
        {
        };
        pInfo_ = &typeid(Void);
    }

    TypeInfo::TypeInfo(const std::type_info& ti) : pInfo_(&ti) { }

    bool TypeInfo::before(const TypeInfo& rhs) const
    {
        return pInfo_->before(*rhs.pInfo_) != 0;
    }

    const std::type_info& TypeInfo::get() const
    {
        return *pInfo_;
    }

    std::string TypeInfo::name() const
    {
        return pInfo_->name();
    }

    bool TypeInfo::operator==(const std::type_info& rhs) const { return this->pInfo_ == &rhs; }
    bool TypeInfo::operator!=(const std::type_info& rhs) const { return !(*this == rhs); }

    bool operator==(const TypeInfo& lhs, const TypeInfo& rhs) { return (lhs.get() == rhs.get()) != 0; }

    bool operator<(const TypeInfo& lhs, const TypeInfo& rhs) { return lhs.before(rhs); }

    bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs) { return !(lhs == rhs); }

    bool operator>(const TypeInfo& lhs, const TypeInfo& rhs) { return rhs < lhs; }

    bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs) { return !(lhs > rhs); }

    bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs) { return !(lhs < rhs); }

}

