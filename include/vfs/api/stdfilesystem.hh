//
// Created by igor on 25/12/2019.
//

#ifndef STDFILESYSTEM_HH
#define STDFILESYSTEM_HH

#include "vfs/vfs_config.h"

#if defined(__cplusplus)
# if !defined(_MSC_VER)
#	if !(__cplusplus > 201402L)
	#	error "C++17 is required"
#	endif
# endif
#endif
#include <bsw/filesystem.hh>

namespace stdfs = bsw::filesystem;

#endif //STDFILESYSTEM_HH
