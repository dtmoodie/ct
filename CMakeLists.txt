cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)
project(ct LANGUAGES CXX VERSION 1.0)
if(NOT MSVC)
  list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
endif()
file(GLOB_RECURSE hdr "include/*.hpp")

add_library(ct INTERFACE)

target_include_directories(ct 
	INTERFACE 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
		$<INSTALL_INTERFACE:include>
)

ADD_CUSTOM_TARGET(ct_files SOURCES ${hdr})
set_target_properties(ct_files PROPERTIES FOLDER Dependencies)

find_package(Boost QUIET COMPONENTS unit_test_framework)
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

if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
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
      cuda_add_executable(${test} ${test_srcs} ${test_knl} ${hdr})
	else()
      add_executable(${test} ${test_srcs} ${hdr})
    endif()
    TARGET_LINK_LIBRARIES(${test} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
	if(OpenCV_FOUND)
	  TARGET_LINK_LIBRARIES(${test} ${OpenCV_LIBS})
	endif()
    TARGET_LINK_LIBRARIES(${test} ct)
    set_target_properties(${test} PROPERTIES FOLDER Tests/ct)
    add_test(${test} ${test})
  endforeach()
endif()

export(TARGETS ct
      FILE "${PROJECT_BINARY_DIR}/ctTargets.cmake"
)

export(PACKAGE ct)

install(TARGETS ct
        EXPORT ctTargets
)

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp"
)
install(EXPORT ctTargets DESTINATION "${CMAKE_INSTALL_PREFIX}/share/ct"
)