include(ExternalProject)

ExternalProject_Add(bsw_project
		GIT_REPOSITORY git@github.com:devbrain/bsw.git
		#GIT_TAG "v2.2.1"
		SOURCE_DIR "${CMAKE_BINARY_DIR}/bsw/bsw-src"
		BINARY_DIR "${CMAKE_BINARY_DIR}/bsw/bsw-build"
		INSTALL_DIR "${CMAKE_BINARY_DIR}/bsw/bsw-install"
		#PATCH_COMMAND git apply "${CMAKE_SOURCE_DIR}/patches/gflags.patch"
		LOG_DOWNLOAD ON
		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		)
