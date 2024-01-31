//
// Created by igor on 25/12/2019.
//

#ifndef VFS_MODULE_LOADER_HH
#define VFS_MODULE_LOADER_HH

#include "vfs/api/vfs_module.h"
#include <filesystem>

#include <memory>

namespace vfs::core
{
	class shared_module
	{
	public:
		static std::unique_ptr<shared_module> load(const std::filesystem::path& pth);

		~shared_module();

		[[nodiscard]] vfs_module_register_t get() const;
		[[nodiscard]] std::filesystem::path path() const;
	private:
		shared_module(void* handle, std::filesystem::path  pth);
		void* _handle;
		std::filesystem::path _path;
	};
}

#endif
