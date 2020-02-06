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

add_library(bsw::bsw SHARED IMPORTED)
set_target_properties(
  bsw::bsw PROPERTIES
  IMPORTED_LOCATION_DEBUG ${BSW_INSTALL_ROOT}/Debug/bsw${LIBRARY_SUFFIX}
  IMPORTED_LOCATION_RELEASE ${BSW_INSTALL_ROOT}/Release/bsw${LIBRARY_SUFFIX}
  IMPORTED_IMPLIB_DEBUG ${BSW_INSTALL_ROOT}/Debug/bsw${CMAKE_IMPORT_LIBRARY_SUFFIX}
  IMPORTED_IMPLIB_RELEASE ${BSW_INSTALL_ROOT}/Release/bsw${CMAKE_IMPORT_LIBRARY_SUFFIX}
  INTERFACE_COMPILE_DEFINITIONS "" 	  
  )
# ==============================================================================
set_target_properties(bsw_ext PROPERTIES FOLDER ${DEPENDENCY_FOLDER})
add_dependencies(vfs_api bsw_ext)