find_package(cpgf QUIET)

if(cpgf_FOUND)
    find_package(OpenCV QUIET COMPONENTS core)

    
    add_executable(test_ct_cpgf main.cpp)
    target_include_directories(test_ct_cpgf ${cpgf_INCLUDE_DIRS})
    target_link_libraries(test_ct_cpgf ${cpgf_LIBRARY})
    if(OpenCV_FOUND)
        target_compile_definitions(test_ct_cpgf HAVE_OPENCV)
        target_include_directories(test_ct_cpgf SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})
    endif()

    find_package(Eigen3 QUIET)
    if(Eigen3_FOUND)
        target_include_directories(test_ct_cpgf SYSTEM PRIVATE ${Eigen3_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})
        target_compile_definitions(test_ct_cpgf HAVE_EIGEN3)
    endif()
endif()
