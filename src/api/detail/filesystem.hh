//
// Created by igor on 25/12/2019.
//

#ifndef FILESYSTEM_HH
#define FILESYSTEM_HH

#include "vfs/api/vfs_module.h"
#include "api/detail/module_loader.hh"

namespace vfs
{
	namespace detail
	{
		class filesystem
		{
		public:
			explicit filesystem(std::unique_ptr<shared_module>&& mdl);




		};
	} // ns detail
} // ns vfs


#endif //FILESYSTEM_HH
