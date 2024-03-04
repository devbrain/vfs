#ifndef VFS_API_STATS_HH
#define VFS_API_STATS_HH

#include <cstdint>
#include <string>
#include <map>

#include <vfs/api/vfs_api.h>

#include <bsw/warn/push.hh>
#include <bsw/warn/dll_interface>
#include <bsw/warn/dll_interface_base>

namespace vfs {
	struct VFS_API stats {
		uint64_t size;
		std::map<std::string, std::string> attribs;

		enum type_t {
			eLINK,
			eFILE,
			eDIRECTORY,
			eNAME_TRUNCATED // if real name was truncated due to maximal path length constraint
		};
		bool is_sequential;
		type_t type;
	};

}

#include <bsw/warn/pop.hh>

#endif
