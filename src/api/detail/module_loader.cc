//
// Created by igor on 25/12/2019.
//

#if !defined(WIN32)
#include <dlfcn.h>
#endif

#include "module_loader.hh"


namespace
{
	void* probe_module(const stdfs::path& pth)
	{
		void* dll = dlopen(pth.c_str(), RTLD_NOW);
		if (!dll)
		{
			return nullptr;
		}
		#define str(s) #s
		if (dlsym(dll, str(VFS_MODULE_REGISTER_NAME)))
		{
			return dll;
		}
		dlclose(dll);
		return nullptr;
	}
}

namespace vfs
{
	namespace detail
	{


		std::unique_ptr<shared_module> shared_module::load(const stdfs::path& pth)
		{
			void* dll = probe_module(pth);
			if (!dll)
			{
				return nullptr;
			}
			return std::unique_ptr<shared_module>(new shared_module(dll));
		}

		shared_module::~shared_module()
		{
			dlclose(_handle);
		}

		vfs_module_register_t shared_module::get()
		{
		  return reinterpret_cast<vfs_module_register_t>(dlsym(_handle, str(VFS_MODULE_REGISTER_NAME)));
		}

		shared_module::shared_module (void* handle)
		: _handle(handle)
		{

		}
	}
}