add_executable(all_the_things main.cpp)

target_link_libraries(all_the_things ct)

if(TARGET cereal OR cereal_INCLUDE_DIR)
    target_compile_definitions(all_the_things PUBLIC HAVE_CEREAL)
endif()
