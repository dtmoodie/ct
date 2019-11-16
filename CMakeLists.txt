cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)
project(ct LANGUAGES CXX VERSION 1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

file(GLOB_RECURSE hdr "include/*.hpp")

include(cmake/subdir_list.cmake)


if(NOT TARGET ct)
    option(BUILD_TESTS "Build tests" ON)
    option(BUILD_EXAMPLES  "Build examples" ON)
    option(DEBUG_TEST_FAILURE "Debug test failure by printing the output of failed static_asserts at runtime" OFF)

    add_library(ct INTERFACE)

    include("${CMAKE_CURRENT_LIST_DIR}/cmake/platform.cmake")
    setTargetPlatformFlags(ct)

    target_include_directories(ct
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )

    # this exists specifically for older version of FindCUDA.cmake which doesn't pull target includes correctly
    set(CT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "" FORCE)

    export(TARGETS ct
        FILE ct-targets.cmake
    )

    install(TARGETS ct 
          EXPORT ct
          DESTINATION lib
    ) # ignored

    install(EXPORT ct FILE ct-config.cmake
        DESTINATION share/ct
    )
    install(DIRECTORY include/ct DESTINATION include)

    if(BUILD_TESTS)
        ADD_SUBDIRECTORY (googletest)
        enable_testing()
        set(GTEST_INCLUDE googletest/googletest/include)

        find_package(OpenCV QUIET COMPONENTS core)
        find_package(CUDA QUIET)

        if(DEBUG_TEST_FAILURE)
            add_definitions(-DDEBUG_CONSTEXPR_OUTPUT)
        endif()

        if(OpenCV_FOUND)
            add_definitions(-DHAVE_OPENCV)
            include_directories(${OpenCV_INCLUDE_DIRS})
        endif()

        if(NOT cereal_INCLUDE_DIR)
            find_package(cereal QUIET)
            if(cereal_FOUND)
                add_definitions(-DHAVE_CEREAL)
            endif()
        else()
            include_directories(SYSTEM ${cereal_INCLUDE_DIR})
            set(cereal_FOUND ON)
        endif()

        if(cereal_FOUND)
            message(STATUS "cereal: found, building cereal tests")
        else()
            message(STATUS "cereal: not found, not building cereal tests")
        endif()

        if(OpenCV_FOUND)
            message(STATUS "OpenCV: found, building OpenCV tests")
        else()
            message(STATUS  "OpenCV: not found, not building OpenCV tests")
        endif()

        SUBDIRLIST(tests "${CMAKE_CURRENT_LIST_DIR}/tests")
        foreach(test ${tests})
            add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests/${test})
        endforeach()

    endif(BUILD_TESTS)

    if(BUILD_EXAMPLES)
        SUBDIRLIST(examples "${CMAKE_CURRENT_LIST_DIR}/examples")
        foreach(example ${examples})
            add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/examples/${example})
        endforeach()
    endif(BUILD_EXAMPLES)
endif()


if(NOT TARGET ct_files)
    ADD_CUSTOM_TARGET(ct_files SOURCES ${hdr})
    set_target_properties(ct_files PROPERTIES FOLDER Dependencies)
endif()


