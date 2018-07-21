file(GLOB src "*.hpp" "*.cpp")
add_executable(test_ct_variadic_typedef ${src})
target_link_libraries(test_ct_variadic_typedef ct)

set_target_properties(test_ct_variadic_typedef PROPERTIES FOLDER Tests/ct)