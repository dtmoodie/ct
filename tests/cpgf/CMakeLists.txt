find_package(cpgf REQUIRED)

if(cpgf_FOUND)
    find_package(OpenCV QUIET COMPONENTS core)
    if(OpenCV_FOUND)
        add_definitions(-DHAVE_OPENCV)
        include_directories(${OpenCV_INCLUDE_DIRS})
    endif()

    find_package(Eigen3 QUIET)
    if(Eigen3_FOUND)
        include_directories(${Eigen3_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})
        add_definitions(-DHAVE_EIGEN3)
    endif()


    include_directories(${cpgf_INCLUDE_DIRS})
    add_executable(test_ct_cpgf main.cpp)
    target_link_libraries(test_ct_cpgf ${cpgf_LIBRARY})
endif()
