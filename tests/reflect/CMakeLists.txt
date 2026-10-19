file(GLOB hdr "*.hpp")

add_executable(test_ct_static_checks ${hdr} static_checks.cpp)
target_link_libraries(test_ct_static_checks ct gtest gtest_main)
target_include_directories(test_ct_static_checks SYSTEM PRIVATE ${GTEST_INCLUDE})
set_target_properties(test_ct_static_checks PROPERTIES FOLDER Tests/ct)
add_test(NAME test_ct_static_checks COMMAND test_ct_static_checks)


find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    include_directories(SYSTEM ${EIGEN3_INCLUDE_DIRS})
    include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})
    include_directories(SYSTEM ${Eigen3_INCLUDE_DIRS})
    include_directories(SYSTEM ${Eigen_INCLUDE_DIRS})
    add_definitions(-DHAVE_EIGEN)
endif()
if(EIGEN3_INCLUDE_DIRS)
    include_directories(SYSTEM ${EIGEN3_INCLUDE_DIRS})
    add_definitions(-DHAVE_EIGEN)
endif()

add_executable(test_ct_reflect_access ${hdr} access.cpp)
target_link_libraries(test_ct_reflect_access ct gtest gtest_main)
target_include_directories(test_ct_reflect_access SYSTEM PRIVATE ${GTEST_INCLUDE})
set_target_properties(test_ct_reflect_access PROPERTIES FOLDER Tests/ct)
add_test(NAME test_ct_reflect_access COMMAND test_ct_reflect_access)

add_executable(test_ct_reflect_print ${hdr} print.cpp)
target_link_libraries(test_ct_reflect_print ct gtest gtest_main)
target_include_directories(test_ct_reflect_print SYSTEM PRIVATE ${GTEST_INCLUDE})
set_target_properties(test_ct_reflect_print PROPERTIES FOLDER Tests/ct)
add_test(NAME test_ct_reflect_print COMMAND test_ct_reflect_print)

if(OpenCV_FOUND)
    target_link_libraries(test_ct_reflect_print ${OpenCV_LIBS})
endif()

add_executable(test_ct_reflect_compare ${hdr} compare.cpp)
target_link_libraries(test_ct_reflect_compare ct gtest gtest_main)
target_include_directories(test_ct_reflect_compare SYSTEM PRIVATE ${GTEST_INCLUDE})
set_target_properties(test_ct_reflect_compare PROPERTIES FOLDER Tests/ct)
add_test(NAME test_ct_reflect_compare COMMAND test_ct_reflect_compare)

if(OpenCV_FOUND)
    target_link_libraries(test_ct_reflect_compare ${OpenCV_LIBS})
endif()

if(cereal_FOUND)
    add_executable(test_ct_reflect_cerealize ${hdr} cerealize.cpp)
    target_link_libraries(test_ct_reflect_cerealize ct gtest gtest_main)
    target_include_directories(test_ct_reflect_cerealize SYSTEM PRIVATE ${GTEST_INCLUDE})
    set_target_properties(test_ct_reflect_cerealize PROPERTIES FOLDER Tests/ct)
    add_test(NAME test_ct_reflect_cerealize COMMAND test_ct_reflect_cerealize)
    if(TARGET cereal)
        target_link_libraries(test_ct_reflect_cerealize cereal)
    endif()
    if(OpenCV_FOUND)
        target_link_libraries(test_ct_reflect_cerealize ${OpenCV_LIBS})
    endif()
endif()

add_executable(test_ct_reflect_hash ${hdr} hash.cpp)
target_link_libraries(test_ct_reflect_hash ct gtest gtest_main)
target_include_directories(test_ct_reflect_hash SYSTEM PRIVATE ${GTEST_INCLUDE})
set_target_properties(test_ct_reflect_hash PROPERTIES FOLDER Tests/ct)
add_test(NAME test_ct_reflect_hash COMMAND test_ct_reflect_hash)


add_executable(test_ct_get_name ${hdr} get_name.cpp)
target_link_libraries(test_ct_get_name ct gtest gtest_main)
target_include_directories(test_ct_get_name SYSTEM PRIVATE ${GTEST_INCLUDE})
set_target_properties(test_ct_get_name PROPERTIES FOLDER Tests/ct)
add_test(NAME test_ct_get_name COMMAND test_ct_get_name)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(test_ct_static_checks PRIVATE -Werror -Wall -Wextra)
    target_compile_options(test_ct_reflect_access PRIVATE -Werror -Wall -Wextra)
    target_compile_options(test_ct_reflect_print PRIVATE -Werror -Wall -Wextra)
    target_compile_options(test_ct_reflect_compare PRIVATE -Werror -Wall -Wextra)
    if(cereal_FOUND)
        # cereal's vendored rapidjson trips -Wdeprecated-declarations (std::iterator) and
        # -Wclass-memaccess on modern GCC; at -O3 GCC also false-positives
        # -Wmaybe-uninitialized on the cereal-loaded test data (it cannot see through
        # the serialization templates that assign the members)
        target_compile_options(test_ct_reflect_cerealize PRIVATE
                               -Werror -Wall -Wextra
                               -Wno-implicit-fallthrough -Wno-deprecated-declarations -Wno-class-memaccess
                               -Wno-maybe-uninitialized)
    endif()
    target_compile_options(test_ct_reflect_hash PRIVATE -Werror -Wall -Wextra)
    target_compile_options(test_ct_get_name PRIVATE -Werror -Wall -Wextra)
endif()

add_executable(test_ct_empty_reflection empty_reflection.cpp)
target_link_libraries(test_ct_empty_reflection  ct gtest gtest_main)
add_test(NAME test_ct_empty_reflection COMMAND test_ct_empty_reflection)