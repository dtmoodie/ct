#ifndef CT_CONFIG_HPP
#define CT_CONFIG_HPP

// Check if we have constexpr support
#define CT_GET_FROM_RVALUE
// clang-format off
#ifdef __CUDA_ARCH__
    #include <cuda_runtime_api.h>
    #if CUDART_VERSION == 6500
        #define CTCONSTEXPR const
    #else
        #define CTCONSTEXPR constexpr
        #define CT_CONSTEXPR_NAME
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
            // There is a known bug where PRETTY_FUNCTION is not constexpr between 5.4 and 8.3
            #if __GNUC__ >= 5 && __GNUC__ < 8
                #define CT_CONSTEXPR_NAME CTCONSTEXPR
                
                //#define CT_HAVE_CONSTEXPR_NAME
            #else
                #define CT_CONSTEXPR_NAME
            #endif
            #if __GNUC__ < 5
                // gcc 4.8 has a bug where this is amgibuous :/
                #undef CT_GET_FROM_RVALUE
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

#if __cplusplus >= 201703L
    #define CT_INLINE_VAR static inline constexpr const
#endif

// before c++17 this is as good as we have :/
#ifndef CT_INLINE_VAR
    #define CT_INLINE_VAR static constexpr const
#endif


#ifdef __CUDA_ARCH__
#include <cuda_runtime_api.h>
#define CT_DEVICE_INLINE __host__ __device__ __inline__
#ifdef CT_HAVE_CONSTEXPR_NAME
#undef CT_HAVE_CONSTEXPR_NAME
#endif
#else
#ifdef _MSC_VER
#define CT_DEVICE_INLINE inline
#else
#define CT_DEVICE_INLINE __inline__
#endif
#endif

// clang-format on

#endif // CT_CONFIG_HPP
