#ifndef CT_REFLECT_MACROS_HPP
#define CT_REFLECT_MACROS_HPP

#define DECL_PRIM(TYPE)                                                                                                \
    template <>                                                                                                        \
    struct ReflectBase<TYPE>                                                                                           \
    {                                                                                                                  \
        static constexpr const bool PRIMITIVE = true;                                                                  \
    };                                                                                                                 \
    template <>                                                                                                        \
    struct Reflect<TYPE> : public ReflectBase<TYPE>                                                                    \
    {                                                                                                                  \
        static constexpr const char* getName() { return "TYPE"; }                                                      \
    }

#endif // CT_REFLECT_MACROS_HPP
