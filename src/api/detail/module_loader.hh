//
// Created by igor on 25/12/2019.
//

#ifndef VFS_MODULE_LOADER_HH
#define VFS_MODULE_LOADER_HH

#include "vfs/api/vfs_module.h"
#include "vfs/api/stdfilesystem.hh"

#include <memory>

namespace vfs::core
{
	class shared_module
	{
	public:
		static std::unique_ptr<shared_module> load(const stdfs::path& pth);

		~shared_module();

		vfs_module_register_t get() const;
		stdfs::path path() const;
	private:
		shared_module(void* handle, const stdfs::path& pth);
		void* _handle;
		stdfs::path _path;
	};
}

#endif
