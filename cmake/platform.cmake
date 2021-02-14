if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0")
endif()

function(setTargetPlatformFlags tgt)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
            target_compile_features(${tgt}
                INTERFACE cxx_return_type_deduction
            )
            #target_compile_options(${tgt} INTERFACE "-Wno-noexcept-type")
        else()
            # for gcc 4.8 we just set the c++1y flag to get return type deduction
            target_compile_options(${tgt} INTERFACE "-std=c++1y")
        endif()

    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        target_compile_features(${tgt}
            INTERFACE cxx_return_type_deduction
        )
        target_compile_options(${tgt} INTERFACE "-lstdc++")
    endif()
endfunction()
