cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)
project(ct LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE hdr "include/*.hpp")

MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()

if(NOT TARGET ct)
    option(BUILD_TESTS "Build tests" ON)
    add_library(ct INTERFACE)

    target_include_directories(ct 
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )

    set(CT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "" FORCE)

    export(TARGETS ct
        FILE ct-targets.cmake
    )
    install(TARGETS ct 
          EXPORT ct
          DESTINATION lib
    ) # ignored

    install(EXPORT ct FILE ct-config.cmake
        DESTINATION share/cmake/ct
    )
    install(DIRECTORY include/ct DESTINATION include)

    if(BUILD_TESTS)
        find_package(Boost QUIET COMPONENTS unit_test_framework)
        find_package(OpenCV QUIET COMPONENTS core)
        INCLUDE_DIRECTORIES(wip)
        if(OpenCV_FOUND)
            add_definitions(-DHAVE_OPENCV)
        endif()
        if(NOT cereal_INCLUDE_DIR)
            find_package(cereal QUIET)
            if(cereal_FOUND)
                add_definitions(-DHAVE_CEREAL)
            endif()
        else()
            add_library(cereal INTERFACE)
                target_include_directories(cereal INTERFACE
                    $<BUILD_INTERFACE:${cereal_INCLUDE_DIR}>
                    $<INSTALL_INTERFACE:include>
                )
            set(cereal_FOUND ON)
        endif()

        if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
            include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/include)
            include(CTest)
            enable_testing()
            SUBDIRLIST(tests "${CMAKE_CURRENT_LIST_DIR}/tests")
            foreach(test ${tests})
                file(GLOB_RECURSE test_srcs "tests/${test}/*.cpp")
                IF(CUDA_FOUND)
                  file(GLOB_RECURSE test_knl "tests/${test}/*.cu")
                ENDIF()
                LIST(LENGTH test_knl num_knl)
                IF(${num_knl} GREATER 0)
                    cuda_add_executable(test_ct_${test} ${test_srcs} ${test_knl} ${hdr})
                else()
                    add_executable(test_ct_${test} ${test_srcs} ${hdr})
                endif()
                TARGET_LINK_LIBRARIES(test_ct_${test} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
                if(OpenCV_FOUND)
                    TARGET_LINK_LIBRARIES(test_ct_${test} ${OpenCV_LIBS})
                endif()
                if(cereal_FOUND)
                    TARGET_LINK_LIBRARIES(test_ct_${test} ct cereal)
                endif()
                set_target_properties(test_ct_${test} PROPERTIES FOLDER Tests/ct)
                add_test(test_ct_${test} test_ct_${test})
            endforeach()
        endif()
        file(GLOB_RECURSE src wip/reflect/*)
        add_executable(wip_reflect ${src})
        target_link_libraries(wip_reflect ct)
        if(OpenCV_FOUND)
            TARGET_LINK_LIBRARIES(wip_reflect ${OpenCV_LIBS})
        endif()
        if(cereal_FOUND)
            TARGET_LINK_LIBRARIES(wip_reflect cereal)
        endif()
    endif(BUILD_TESTS)

endif()

if(NOT TARGET ct_files)
    ADD_CUSTOM_TARGET(ct_files SOURCES ${hdr})
    set_target_properties(ct_files PROPERTIES FOLDER Dependencies)
endif()


