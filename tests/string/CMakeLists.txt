
add_executable(test_ct_string_static static_test.cpp)
target_link_libraries(test_ct_string_static ct)
set_target_properties(test_ct_string_static PROPERTIES FOLDER Tests/ct)
add_test(test_ct_string_static test_ct_string_static)


add_executable(test_ct_string_runtime runtime_test.cpp)
target_link_libraries(test_ct_string_runtime ct gtest gtest_main)
set_target_properties(test_ct_string_runtime PROPERTIES FOLDER Tests/ct)
add_test(test_ct_string_runtime test_ct_string_runtime)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(test_ct_string_static PRIVATE -Werror -Wall -Wextra)
    target_compile_options(test_ct_string_runtime PRIVATE -Werror -Wall -Wextra)
endif()
