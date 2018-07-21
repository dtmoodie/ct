file(GLOB src "*.hpp" "*.cpp")
add_executable(test_ct_type_traits ${src})
target_link_libraries(test_ct_type_traits ct)

set_target_properties(test_ct_type_traits PROPERTIES FOLDER Tests/ct)