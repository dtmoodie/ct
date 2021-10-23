set(POSTFIX "cpp")


function(add_conformance_test)
    set(options CUDA)
    set(one_value_args NAME)
    cmake_parse_arguments(add_conformance_test "${options}" "${one_value_args}" "" ${ARGN})

    string(SUBSTRING "${add_conformance_test_NAME}" "3" "-1" NAME)

    if(add_conformance_test_CUDA)
        cuda_add_executable(${NAME}_cuda ${add_conformance_test_NAME}.cu)
        target_link_libraries(${NAME}_cuda PUBLIC ct)
        set_target_properties(${NAME}_cuda PROPERTIES FOLDER Tests/ct_cpp_compat)

    else()
        add_executable(${NAME}_cpp ${add_conformance_test_NAME}.cpp)
        target_link_libraries(${NAME}_cpp PRIVATE ct)
        set_target_properties(${NAME}_cpp PROPERTIES FOLDER Tests/ct_cuda_compat)
    endif()
endfunction()

add_conformance_test(NAME 00_compile_time_string_view)
add_conformance_test(NAME 01_compile_time_string_hash)
add_conformance_test(NAME 02_compile_time_string_search)
add_conformance_test(NAME 03_reflect_external)
add_conformance_test(NAME 04_reflect_internal)
add_conformance_test(NAME 05_enum_external)
add_conformance_test(NAME 06_enum_internal)
        

find_package(CUDA QUIET)
if(CUDA_FOUND)
    set(CUDA_NVCC_FLAGS "-std=c++11")
    set(CUDA_LINK_LIBRARIES_KEYWORD PUBLIC)
    include_directories("${CMAKE_CURRENT_LIST_DIR}/../../include")
    add_conformance_test(NAME 00_compile_time_string_view CUDA)
    add_conformance_test(NAME 01_compile_time_string_hash CUDA)
    add_conformance_test(NAME 02_compile_time_string_search CUDA)
    add_conformance_test(NAME 03_reflect_external CUDA)
    add_conformance_test(NAME 04_reflect_internal CUDA)
    add_conformance_test(NAME 05_enum_external CUDA)
    add_conformance_test(NAME 06_enum_internal CUDA)
endif()