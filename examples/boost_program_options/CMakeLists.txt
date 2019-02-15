
if(Boost_VERSION VERSION_LESS "106800")
    find_package(Boost QUIET COMPONENTS program_options)
    if(Boost_FOUND)
        add_executable(test_ct_boost main.cpp)
        target_link_libraries(test_ct_boost ${Boost_LIBRARIES} ct)
    endif()
endif()
