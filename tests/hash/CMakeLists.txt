
add_executable(test_ct_string_hash string_hash.cpp)
set_target_properties(test_ct_string_hash PROPERTIES FOLDER Tests/ct)
target_link_libraries(test_ct_string_hash ct)


add_executable(test_ct_object_hash object_hash.cpp)
set_target_properties(test_ct_object_hash PROPERTIES FOLDER Tests/ct)
target_link_libraries(test_ct_object_hash ct)
