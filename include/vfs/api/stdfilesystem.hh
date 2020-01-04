//
// Created by igor on 25/12/2019.
//

#ifndef STDFILESYSTEM_HH
#define STDFILESYSTEM_HH

#include "vfs/vfs_config.h"

#if defined(__cplusplus)
#	if !(__cplusplus > 201402L)
	#	error "C++17 is required"
#	endif
#endif

#include VFS_FS_HEADER
#define stdfs  VFS_FS_NAMESPACE

#endif //STDFILESYSTEM_HH
