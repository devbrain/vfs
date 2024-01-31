#ifndef VFS_API_EXCEPTION_HH
#define VFS_API_EXCEPTION_HH

#include <bsw/exception.hh>
#include <vfs/api/vfs_api.h>
#include <stdexcept>
#include <string>

namespace vfs {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning( disable : 4275 )
#endif

	class VFS_API exception : public bsw::exception {
	 public:
		template <typename ... Args>
		exception (const char* function, const char* source, int line, Args&& ... args)
			: bsw::exception (function, source, line, std::forward<Args> (args)...) {}

		template <typename ... Args>
		exception (exception cause, const char* function, const char* source, int line, Args&& ... args)
			: bsw::exception (cause, function, source, line, std::forward<Args> (args)...) {}

	};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#define THROW_EXCEPTION_EX(Exception, ...) RAISE_EX_CUSTOM(Exception, ##__VA_ARGS__)
} // ns vfs

#endif
