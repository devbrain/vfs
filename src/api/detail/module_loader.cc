//
// Created by igor on 25/12/2019.
//

#include "module_loader.hh"

#if !defined(WIN32)
#include <dlfcn.h>
#else 
#include <windows.h>

#define RTLD_LAZY 0

namespace
{
	void* dlopen(const std::filesystem::path::value_type* path, int)
	{
		return LoadLibraryW(path);
	}

	void* dlsym(void* dll,const char* symbol)
	{
		return GetProcAddress(reinterpret_cast<HMODULE>(dll), symbol);
	}

	void dlclose(void* dll)
	{
		FreeLibrary(reinterpret_cast<HMODULE>(dll));
	}

}

#endif




namespace
{
	void* probe_module(const stdfs::path& pth)
	{
		void* dll = dlopen(pth.c_str(), RTLD_LAZY);
		if (!dll)
		{
			return nullptr;
		}

		if (dlsym(dll, VFS_MODULE_REGISTER_NAME))
		{
			return dll;
		}
		std::string err = dlerror();
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
		  return reinterpret_cast<vfs_module_register_t>(dlsym(_handle, VFS_MODULE_REGISTER_NAME));
		}

		shared_module::shared_module (void* handle)
		: _handle(handle)
		{

		}
	}
}