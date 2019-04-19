set(cpgf_DIR "" CACHE PATH "cpfg dir")

find_library(cpgf_LIBRARY
    NAME cpgf
    HINTS ${cpgf_DIR}/lib
)

find_path(cpgf_INCLUDE_DIRS
    NAME cpgf/gapi.h
    HINTS ${cpgf_DIR}/include
)

if(cpgf_INCLUDE_DIRS AND cpgf_LIBRARY)
    set(cpgf_FOUND ON)
else()
    set(cpgf_FOUND OFF)
endif()
