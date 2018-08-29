
if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
    add_executable(test_ct_string_runtime boost_test.cpp)
    target_link_libraries(test_ct_string_runtime ct ${Boost_LIBRARIES})
    set_target_properties(test_ct_string_runtime PROPERTIES FOLDER Tests/ct)
endif()
add_executable(test_ct_string_static static_test.cpp)
target_link_libraries(test_ct_string_static ct)
set_target_properties(test_ct_string_static PROPERTIES FOLDER Tests/ct)