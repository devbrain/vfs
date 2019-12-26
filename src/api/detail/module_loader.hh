//
// Created by igor on 25/12/2019.
//

#ifndef MODULE_LOADER_HH
#define MODULE_LOADER_HH

#include "vfs/api/vfs_module.h"
#include "api/detail/stdfilesystem.hh"

#include <memory>

namespace vfs
{
	namespace detail
	{
		class shared_module
		{
		public:
			static std::unique_ptr<shared_module> load(const stdfs::path& pth);

			~shared_module();

			vfs_module_register_t get();
		private:
			shared_module (void* handle);
			void* _handle;
		};
	}
}

#endif //MODULE_LOADER_HH
