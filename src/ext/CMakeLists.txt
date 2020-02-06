include(ExternalProject)

if (CMAKE_CONFIGURATION_TYPES)
	set(propagate_build_type --config $<CONFIG>)
else()
	set(propagate_build_type -DCMAKE_BUILD_TYPE=$<CONFIG>)
endif()

set (BSW_INSTALL_ROOT ${CMAKE_BINARY_DIR}/ext/bsw)
ExternalProject_Add(bsw_ext
		GIT_REPOSITORY git@github.com:devbrain/bsw.git
		INSTALL_DIR    ${BSW_INSTALL_ROOT}
		CMAKE_ARGS 
			-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
			-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}
			${propagate_build_type}
		UPDATE_COMMAND    ""
		BUILD_ALWAYS      OFF
	)

add_library(bsw::bsw SHARED IMPORTED GLOBAL)

if (WINDOWS)
  set (BSW_LIB bsw.dll)
else ()
  set (BSW_LIB libbsw.so)
endif()


if (NOT IS_SINGLE_CONFIGURATION)
set_target_properties(
  bsw::bsw PROPERTIES
  IMPORTED_LOCATION_DEBUG   ${BSW_INSTALL_ROOT}/lib/Debug/${BSW_LIB}
  IMPORTED_LOCATION_RELEASE ${BSW_INSTALL_ROOT}/lib/Release/${BSW_LIB}
  IMPORTED_IMPLIB_DEBUG     ${BSW_INSTALL_ROOT}/lib/Debug/bsw${CMAKE_IMPORT_LIBRARY_SUFFIX}
  IMPORTED_IMPLIB_RELEASE   ${BSW_INSTALL_ROOT}/lib/Release/bsw${CMAKE_IMPORT_LIBRARY_SUFFIX}
  INTERFACE_COMPILE_DEFINITIONS ""
  INTERFACE_INCLUDE_DIRECTORIES ${BSW_INSTALL_ROOT}/include
  )
else ()
  set_target_properties(
  bsw::bsw PROPERTIES
  IMPORTED_LOCATION   ${BSW_INSTALL_ROOT}/lib/${CMAKE_BUILD_TYPE}/${BSW_LIB}
  IMPORTED_IMPLIB     ${BSW_INSTALL_ROOT}/lib/${CMAKE_BUILD_TYPE}/bsw${CMAKE_IMPORT_LIBRARY_SUFFIX}
  INTERFACE_INCLUDE_DIRECTORIES ${BSW_INSTALL_ROOT}/include
  INTERFACE_COMPILE_DEFINITIONS "" 	  
  )
endif()
# ==============================================================================
set_target_properties(bsw_ext PROPERTIES FOLDER ${DEPENDENCY_FOLDER})
add_dependencies(vfs_api bsw_ext)
