find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    include_directories(${EIGEN3_INCLUDE_DIRS})
    include_directories(${EIGEN3_INCLUDE_DIR})
    include_directories(${Eigen3_INCLUDE_DIRS})
    include_directories(${Eigen_INCLUDE_DIRS})
    add_definitions(-DHAVE_EIGEN)
endif()
if(EIGEN3_INCLUDE_DIRS)
    include_directories(${EIGEN3_INCLUDE_DIRS})
    add_definitions(-DHAVE_EIGEN)
endif()

add_library(test_ct_python SHARED main.cpp)
target_link_libraries(test_ct_python
    optimized;${Boost_PYTHON_LIBRARY_RELEASE}
    debug;${Boost_PYTHON_LIBRARY_DEBUG}
    ${PYTHON_LIBRARY}
    ct
)

if(OpenCV_FOUND)
    target_link_libraries(test_ct_python ${OpenCV_LIBS})
endif()

target_include_directories(test_ct_python
    PUBLIC
        $<BUILD_INTERFACE:${PYTHON_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${NUMPY_INCLUDE_DIR}>
)

get_target_property(out_dir test_ct_python ARCHIVE_OUTPUT_DIRECTORY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/test_ct_python ${out_dir}/test_ct_python @ONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/test_ct_python.py ${out_dir}/test_ct_python.py @ONLY)