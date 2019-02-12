#ifndef CT_REFLECT_METADATA_HPP
#define CT_REFLECT_METADATA_HPP

namespace ct
{
    struct Description
    {
        constexpr Description(const char* desc) : m_desc(desc) {}

        const char* m_desc;
    };
}

#endif // CT_REFLECT_METADATA_HPP
