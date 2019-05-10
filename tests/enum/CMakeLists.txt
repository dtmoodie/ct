add_executable(test_ct_enum enum.cpp)
target_link_libraries(test_ct_enum ct)
set_target_properties(test_ct_enum PROPERTIES FOLDER Tests/ct)
