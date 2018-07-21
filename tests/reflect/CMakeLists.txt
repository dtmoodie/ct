file(GLOB hdr "*.hpp")

add_executable(test_ct_reflect_access ${hdr} access.cpp)
target_link_libraries(test_ct_reflect_access ct)
set_target_properties(test_ct_reflect_access PROPERTIES FOLDER Tests/ct)

add_executable(test_ct_reflect_print ${hdr} print.cpp)
target_link_libraries(test_ct_reflect_print ct)
set_target_properties(test_ct_reflect_print PROPERTIES FOLDER Tests/ct)