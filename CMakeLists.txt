cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)
project(ct LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


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
    option(DEBUG_TEST_FAILURE "Debug test failure by printing the output of failed static_asserts at runtime" OFF)
    file(GLOB_RECURSE src "src/*")
    include_directories(include)
    add_library(ct SHARED ${src})

    target_include_directories(ct
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )

    set_target_properties(ct
        PROPERTIES
            CXX_STANDARD 11
    )

    set_target_properties(ct
        PROPERTIES
            CXX_STANDARD_REQUIRED ONx
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
        include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
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
            include_directories(${cereal_INCLUDE_DIR})
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

        if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
            add_definitions(-DHAVE_BOOST)
            include_directories(${Boost_INCLUDE_DIRS})
            message(STATUS "Found boost test, building unit tests")
        else()
            message(STATUS "Boost unit test: not found, not buildng most tests")
        endif()
        
        include(CTest)
        enable_testing()
        SUBDIRLIST(tests "${CMAKE_CURRENT_LIST_DIR}/tests")
        foreach(test ${tests})
            add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests/${test})
        endforeach()
        
    endif(BUILD_TESTS)
endif()

if(NOT TARGET ct_files)
    ADD_CUSTOM_TARGET(ct_files SOURCES ${hdr})
    set_target_properties(ct_files PROPERTIES FOLDER Dependencies)
endif()


