#ifndef VFS_API_STATS_HH
#define VFS_API_STATS_HH

#include <cstdint>
#include <string>
#include <map>

#include <vfs/api/vfs_api.h>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning( disable : 4275 4251 )
#endif

namespace vfs
{
	struct VFS_API stats
	{
		uint64_t size;
		std::map<std::string, std::string> attribs;

		enum type_t
		{
			eLINK,
			eFILE,
			eDIRECTORY
		};

		type_t type;
	};

}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
