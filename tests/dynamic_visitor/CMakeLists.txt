file(GLOB hdr "*.hpp")

add_executable(test_ct_dynamic_access ${hdr} print.cpp)
target_link_libraries(test_ct_dynamic_access ct)
set_target_properties(test_ct_dynamic_access PROPERTIES FOLDER Tests/ct)

