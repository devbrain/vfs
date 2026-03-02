#ifndef VFS_API_EXCEPTION_HH
#define VFS_API_EXCEPTION_HH

#include <failsafe/exception.hh>
#include "vfs/api/vfs_api.h"
#include <stdexcept>
#include <string>

namespace vfs {
#include "bsw/warn/push.hh"
#include "bsw/warn/dll_interface"

	class VFS_API exception : public std::runtime_error {
	 public:
		template <typename ... Args>
		exception (Args&& ... args)
			: std::runtime_error (std::forward<Args> (args)...) {}
	};

#include "bsw/warn/pop.hh"
#define THROW_EXCEPTION_EX(Exception, ...) THROW(Exception, __VA_ARGS__)
} // ns vfs

#endif
