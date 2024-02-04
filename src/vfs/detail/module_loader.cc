//
// Created by igor on 25/12/2019.
//

#include "module_loader.hh"
#include <bsw/dlfcn.hh>
#include <utility>

namespace {
	void* probe_module (const std::filesystem::path& pth) {
		std::string str = pth.string ();
		void* dll = dlopen (str.c_str (), RTLD_LAZY);
		if (!dll) {
			[[maybe_unused]]const char* error_msg = dlerror ();

			return nullptr;
		}

		if (dlsym (dll, VFS_MODULE_REGISTER_NAME)) {
			return dll;
		}

		dlclose (dll);
		return nullptr;
	}
}

namespace vfs::core {

	std::unique_ptr<shared_module> shared_module::load (const std::filesystem::path& pth) {

		void* dll = probe_module (pth);
		if (!dll) {
			return nullptr;
		}
		return std::unique_ptr<shared_module> (new shared_module (dll, pth));
	}

	// --------------------------------------------------------------------------------
	shared_module::~shared_module () {
		dlclose (_handle);
	}

	// --------------------------------------------------------------------------------
	vfs_module_register_t shared_module::get () const {
		return reinterpret_cast<vfs_module_register_t>(dlsym (_handle, VFS_MODULE_REGISTER_NAME));
	}

	// --------------------------------------------------------------------------------
	std::filesystem::path shared_module::path () const {
		return _path;
	}

	// --------------------------------------------------------------------------------
	shared_module::shared_module (void* handle, std::filesystem::path  pth)
		: _handle (handle),
		  _path (std::move(pth)) {

	}

} // ns vfs::core
