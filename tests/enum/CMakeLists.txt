add_executable(test_ct_enum enum.cpp)
target_link_libraries(test_ct_enum ct gtest gtest_main)
target_include_directories(test_ct_enum SYSTEM PRIVATE ${GTEST_INCLUDE})
set_target_properties(test_ct_enum PROPERTIES FOLDER Tests/ct)
add_test(test_ct_enum test_ct_enum)

if(CUDA_FOUND)
    set(CUDA_NVCC_FLAGS "-std=c++11")
    set(CUDA_LINK_LIBRARIES_KEYWORD PUBLIC)
    include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include")
    cuda_add_executable(test_ct_enum_cuda enum.cu)
    target_link_libraries(test_ct_enum_cuda 
        PUBLIC
            ct 
            gtest 
            gtest_main
    )
    target_include_directories(test_ct_enum_cuda SYSTEM PRIVATE ${GTEST_INCLUDE})
    set_target_properties(test_ct_enum_cuda PROPERTIES FOLDER Tests/ct)
    add_test(test_ct_enum_cuda test_ct_enum_cuda)
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(test_ct_enum PRIVATE -Werror -Wall -Wextra)
    if(CUDA_FOUND)
        target_compile_options(test_ct_enum_cuda PRIVATE -Werror -Wall -Wextra)
    endif()
endif()
