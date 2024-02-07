add_library(popl INTERFACE popl.hpp)
target_include_directories(popl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(popl INTERFACE bsw)