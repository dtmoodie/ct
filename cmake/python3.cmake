if(WIN32)
find_package(Boost QUIET COMPONENTS python python2)
    if(Boost_PYTHON_FOUND)
        find_package(NumPy 1.7.1)
        if(NUMPY_FOUND)
            set(Boost_PYTHON_LIBRARY_RELEASE "${Boost_PYTHON-PY${boost_py_version}_LIBRARY_RELEASE}" CACHE PATH "" FORCE)
            set(Boost_PYTHON_LIBRARY_DEBUG "${Boost_PYTHON-PY${boost_py_version}_LIBRARY_DEBUG}" CACHE PATH "" FORCE)
            set(HAVE_PYTHON 1 CACHE BOOL INTERNAL FORCE)
            set(PYTHON_STATUS "${PYTHONLIBS_VERSION_STRING} include: ${PYTHON_INCLUDE_DIR} Boost: ${Boost_PYTHON_LIBRARY_RELEASE} Numpy: ${NUMPY_VERSION}" CACHE BOOL INTERNAL FORCE)
            list(APPEND link_libs "${Boost_PYTHON_LIBRARY_RELEASE};${PYTHON_LIBRARY}")
            get_filename_component(_python_lib_dirs "${PYTHON_LIBRARY}" DIRECTORY)
            foreach(dir ${_python_lib_dirs})
                if(NOT ${dir} STREQUAL debug AND NOT ${dir} STREQUAL optimized)
                    link_directories(${dir})
                endif()
            endforeach()
            set(HAVE_PYTHON 1 CACHE BOOL INTERNAL FORCE)
        else()
            set(PYTHON_STATUS "Unable to find numpy" CACHE BOOL INTERNAL FORCE)
        endif()
    endif()
else(WIN32)
    # Find the matching boost python implementation
    set(searched_libs "")
    set(version ${Python_VERSION})
    
    STRING( REGEX REPLACE "[^0-9]" "" boost_py_version ${version} )
    find_package(Boost QUIET COMPONENTS python)
    if(Boost_PYTHON_FOUND)
        #message(${Boost_PYTHON_LIBRARY})
    endif()

    # older versions of boost call it boost-py
    while(NOT "${version}" STREQUAL "" AND NOT Boost_PYTHON_FOUND)
        STRING( REGEX REPLACE "([0-9.]+).[0-9]+" "\\1" version ${version} )

        STRING( REGEX REPLACE "[^0-9]" "" boost_py_version ${version} )
        find_package(Boost QUIET COMPONENTS "python-py${boost_py_version}")
        list(APPEND searched_libs "python-PY${boost_py_version}")
        set(Boost_PYTHON_FOUND ${Boost_FOUND})


        STRING( REGEX MATCHALL "([0-9.]+).[0-9]+" has_more_version ${version} )
        if("${has_more_version}" STREQUAL "" OR Boost_PYTHON_FOUND)
            set(Boost_PYTHON_LIBRARY_RELEASE "${Boost_PYTHON-PY${boost_py_version}_LIBRARY_RELEASE}" CACHE PATH "" FORCE)
            set(Boost_PYTHON_LIBRARY_DEBUG "${Boost_PYTHON-PY${boost_py_version}_LIBRARY_DEBUG}" CACHE PATH "" FORCE)
            break()
        endif()
    endwhile()

    # newer versions of boost call it boost-python
    set(version ${PYTHONLIBS_VERSION_STRING})
    if(NOT Boost_PYTHON_FOUND)
        while(NOT "${version}" STREQUAL "" AND NOT Boost_PYTHON_FOUND)
            STRING( REGEX REPLACE "([0-9.]+).[0-9]+" "\\1" version ${version} )

            STRING( REGEX REPLACE "[^0-9]" "" boost_py_version ${version} )
            find_package(Boost 1.46 QUIET COMPONENTS "python${boost_py_version}")
            list(APPEND searched_libs "python${boost_py_version}")
            set(Boost_PYTHON_FOUND ${Boost_FOUND})

            STRING( REGEX MATCHALL "([0-9.]+).[0-9]+" has_more_version ${version} )
            if("${has_more_version}" STREQUAL "" OR Boost_PYTHON_FOUND)
                set(Boost_PYTHON_LIBRARY_RELEASE "${Boost_PYTHON${boost_py_version}_LIBRARY_RELEASE}" CACHE PATH "" FORCE)
                set(Boost_PYTHON_LIBRARY_DEBUG "${Boost_PYTHON${boost_py_version}_LIBRARY_DEBUG}" CACHE PATH "" FORCE)
                break()
            endif()
        endwhile()
    endif()

    if(Boost_PYTHON_FOUND)
        if(Python_NumPy_FOUND)
            set(HAVE_PYTHON 1 CACHE BOOL INTERNAL FORCE)
            set(PYTHON_STATUS "${PYTHONLIBS_VERSION_STRING} include: ${PYTHON_INCLUDE_DIR} Boost: ${Boost_PYTHON_LIBRARY_RELEASE} Numpy: ${Python3_NumPy_VERSION}" CACHE BOOL INTERNAL FORCE)
        else()
            set(PYTHON_STATUS "Unable to find numpy" CACHE BOOL INTERNAL FORCE)
        endif()
    else()
        set(PYTHON_STATUS "Unable to find a suitable version of boost python ${searched_libs}" CACHE BOOL INTERNAL FORCE)
    endif()
endif(WIN32)