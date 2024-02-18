set(ENABLE_TEST FALSE CACHE INTERNAL "")


# SET(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE INTERNAL "")

fetchcontent_declare(
        LIBARCHIVE_EXT
        GIT_REPOSITORY https://github.com/libarchive/libarchive
        GIT_PROGRESS TRUE
        GIT_TAG master
        TIMEOUT 10
        UPDATE_COMMAND ""
)

fetchcontent_makeavailable(LIBARCHIVE_EXT)

