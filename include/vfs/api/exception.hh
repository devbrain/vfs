#ifndef VFS_API_EXCEPTION_HH
#define VFS_API_EXCEPTION_HH

#include <vfs/api/vfs_api.h>
#include <stdexcept>
#include <string>

namespace vfs
{
	class VFS_API exception : public std::runtime_error
	{
	public:
		exception(const std::string& err)
		: std::runtime_error(err)
		{

		}
	};
} // ns vfs

#endif
