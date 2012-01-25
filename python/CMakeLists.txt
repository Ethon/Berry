# Include.
include_directories( /usr/include/python2.7 )

# Compile as shared library.
add_library( pyberry SHARED
    pyberry.cpp
    export_process_api.cpp
)

# Link libraries.
target_link_libraries( pyberry
    berry
    python2.7
    boost_system
    boost_filesystem
    boost_python
)
