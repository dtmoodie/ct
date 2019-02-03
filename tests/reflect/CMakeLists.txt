file(GLOB hdr "*.hpp")

add_executable(test_ct_reflect_access ${hdr} access.cpp)
target_link_libraries(test_ct_reflect_access ct)
set_target_properties(test_ct_reflect_access PROPERTIES FOLDER Tests/ct)

add_executable(test_ct_reflect_print ${hdr} print.cpp)
target_link_libraries(test_ct_reflect_print ct)
set_target_properties(test_ct_reflect_print PROPERTIES FOLDER Tests/ct)

add_executable(test_ct_reflect_compare ${hdr} compare.cpp)
target_link_libraries(test_ct_reflect_compare ct)
set_target_properties(test_ct_reflect_compare PROPERTIES FOLDER Tests/ct)

if(cereal_FOUND)
    add_executable(test_ct_reflect_cerealize ${hdr} cerealize.cpp)
    target_link_libraries(test_ct_reflect_cerealize ct)
    set_target_properties(test_ct_reflect_cerealize PROPERTIES FOLDER Tests/ct)
    if(TARGET cereal)
        target_link_libraries(test_ct_reflect_cerealize cereal)
    endif()
endif()
add_executable(test_ct_reflect_hash ${hdr} hash.cpp)
target_link_libraries(test_ct_reflect_hash ct)
set_target_properties(test_ct_reflect_hash PROPERTIES FOLDER Tests/ct)

add_executable(test_ct_data_table ${hdr} datatable.cpp)
target_link_libraries(test_ct_data_table ct)
set_target_properties(test_ct_data_table PROPERTIES FOLDER Tests/ct)

add_executable(test_ct_static_checks ${hdr} static_checks.cpp)
target_link_libraries(test_ct_static_checks ct)
set_target_properties(test_ct_static_checks PROPERTIES FOLDER Tests/ct)
