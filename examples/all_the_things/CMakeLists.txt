add_executable(all_the_things main.cpp)

target_link_libraries(all_the_things ct)

if(TARGET cereal OR cereal_INCLUDE_DIR)
    target_compile_definitions(all_the_things PUBLIC HAVE_CEREAL)
endif()

find_package(Boost QUIET COMPONENTS program_options)

if(Boost_FOUND)
    target_compile_definitions(all_the_things PUBLIC HAVE_BOOST)
    target_link_libraries(all_the_things ${Boost_LIBRARIES})
endif()
