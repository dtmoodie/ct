function(setTargetPlatformFlags tgt)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
            target_compile_features(${tgt}
                INTERFACE cxx_return_type_deduction
            )
        else()
            # for gcc 4.8 we just set the c++1y flag to get return type deduction
            target_compile_options(ct INTERFACE "-std=c++1y")
        endif()

    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_STANDARD 14)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
    endif()
endfunction()
