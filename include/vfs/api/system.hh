//
// Created by igor on 26/12/2019.
//

#ifndef SYSTEM_HH
#define SYSTEM_HH


#include "vfs/api/stdfilesystem.hh"
#include "vfs/api/vfs_api.h"

#include <memory>

namespace vfs
{
	namespace detail
	{
		class VFS_API system
		{
		public:
			explicit system(const stdfs::path& modules_path);
			~system();
		private:
			struct impl;
			std::unique_ptr<impl> _impl;
		};
	} // ns detail
} // ns vfs

#endif //SYSTEM_HH
