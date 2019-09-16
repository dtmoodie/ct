
find_package(Boost QUIET COMPONENTS program_options)
if(Boost_FOUND)
    add_executable(example_ct_boost main.cpp)
    target_link_libraries(example_ct_boost ${Boost_LIBRARIES} ct)
    target_include_directories(example_ct_boost PRIVATE SYSTEM ${Boost_INCLUDE_DIRS})
endif()
