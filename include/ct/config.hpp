#ifndef CT_CONFIG_HPP
#define CT_CONFIG_HPP

// Check if we have constexpr support

// clang-format off
#ifdef __CUDA_ARCH__
#include <cuda_runtime_api.h>
    #if CUDART_VERSION == 6500
        #define CTCONSTEXPR const
    #else
        #define CTCONSTEXPR constexpr
    #endif

#else

    #ifdef _MSC_VER
        #define CT_FUNCTION_NAME __FUNCTION__
        #define CT_CONSTEXPR_NAME constexpr
        #define CT_HAVE_CONSTEXPR_NAME
    #else // _MSC_VER
        #ifdef __GNUC__
            #define CT_FUNCTION_NAME __PRETTY_FUNCTION__

            #if __cplusplus >= 201100
                #define CTCONSTEXPR constexpr
            #else // __cplusplus > 2011
                #define CTCONSTEXPR
            #endif

            #if __GNUC__ >= 5
                #define CT_CONSTEXPR_NAME CTCONSTEXPR
                #define CT_HAVE_CONSTEXPR_NAME

            #else
                #define CT_CONSTEXPR_NAME
            #endif

        #else // __GNUC__
            // not supported platform yet
        #endif // __GNUC__

    #endif // _MSC_VER

#endif // __CUDA_ACC__
#ifndef CT_FUNCTION_NAME
#define CT_FUNCTION_NAME ""
#endif
#ifndef CT_CONSTEXPR_NAME
#define CT_CONSTEXPR_NAME
#endif

// clang-format on

#endif // CT_CONFIG_HPP
