cmake_policy(SET CMP0078 NEW)
cmake_policy(SET CMP0086 NEW)
find_package(SWIG)
if (SWIG_FOUND)
    include(${SWIG_USE_FILE})

    # Add subdirectories for each language if desired
    option(BUILD_PYTHON "Build Python SWIG module" ON)
    if(BUILD_PYTHON)
        add_subdirectory(python)
    endif()
else()
    message("No Swig found")
endif()
