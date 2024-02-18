//
// Created by igor on 26/12/2019.
//

#include "modules_table.hh"
#include "physfs/physfs.hh"
#include "vfs/exception.hh"

namespace vfs::core {
	modules_table::entry::entry (vfs_module* obj, std::unique_ptr<shared_module>&& dll)
		: _ref_count (1),
		  _fs (new file_system (obj)),
		  _dll (std::move (dll)) {

	}

	// ------------------------------------------------------------------------------------
	void modules_table::entry::inc_ref () {
		_ref_count++;
	}

	// ------------------------------------------------------------------------------------
	bool modules_table::entry::dec_ref () {
		_ref_count--;
		if (_ref_count == 0) {
			delete this;
			return false;
		}
		return true;
	}

	// ------------------------------------------------------------------------------------
	std::filesystem::path modules_table::entry::path () const {
		if (_dll) {
			return _dll->path ();
		}
		return {};
	}

	// ------------------------------------------------------------------------------------
	int modules_table::entry::ref_count () const {
		return _ref_count;
	}

	// ------------------------------------------------------------------------------------
	file_system* modules_table::entry::module () const {
		return _fs;
	}
	// ------------------------------------------------------------------------------------
	modules_table::entry::~entry () {
		delete _fs;
	}
	// ===================================================================================
	modules_table::modules_table (const std::filesystem::path& path) {
		_add_module (new vfs::detail::physfs);
		add (path, nullptr);
	}
	// -----------------------------------------------------------------------------------
	modules_table::modules_table () {
		_add_module (new vfs::detail::physfs);
	}
	// -----------------------------------------------------------------------------------
	modules_table::modules_table(std::unique_ptr<vfs::module::filesystem> fsptr, bool register_phys_fs) {
		if (register_phys_fs) {
			_add_module (new vfs::detail::physfs);
		}
		if (fsptr) {
			add (std::move (fsptr));
		}
	}
	// -----------------------------------------------------------------------------------
	void modules_table::add (std::unique_ptr<vfs::module::filesystem> fsptr) {
		auto* fs = std::move(fsptr).release();
		_add_module (fs);
	}
	// -----------------------------------------------------------------------------------
	void modules_table::_add_module(vfs::module::filesystem* fs) {
		std::unique_ptr<vfs_module> impl_module = std::make_unique<vfs_module> ();
		fs->setup (impl_module.get ());
		if (impl_module->get_name) {
			char name[128] = {0};
			impl_module->get_name (impl_module->opaque, name, sizeof (name));
			_entries[name] = new entry (impl_module.release (), nullptr);
		}
	}
	// -----------------------------------------------------------------------------------
	modules_table::~modules_table () {
		for (const auto& e : _entries) {
			delete e.second;
		}
	}

	// -----------------------------------------------------------------------------------
	void modules_table::add (const std::filesystem::path& path, modules_loading_report* report) {
		if (std::filesystem::is_directory (path)) {
			for (auto& p : std::filesystem::directory_iterator (path)) {
				add (p, report);
			}
		} else {
			if (std::filesystem::is_regular_file (path)) {
#if defined(_WIN32) || defined(WIN32)
#define DLL_EXT  L".dll"
				using ext_type = std::wstring;
#else
#define DLL_EXT  ".so"
				using ext_type = std::string;
#endif
				ext_type ext = path.extension ().c_str ();
				ext_type xxx = path.c_str ();
				if (ext == DLL_EXT) {
					_add_file (path, report);
				}
			}
		}
	}

	// -----------------------------------------------------------------------------------
	void modules_table::_add_file (const std::filesystem::path& path, modules_loading_report* report) {
		std::unique_ptr<shared_module> shm = shared_module::load (path);
		bool loaded = false;
		if (shm) {
			auto creator = shm->get ();

			std::unique_ptr<vfs_module> impl_module = std::make_unique<vfs_module> ();

			creator (impl_module.get ());

			if (impl_module->get_name) {
				char name[128] = {0};
				impl_module->get_name (impl_module->opaque, name, sizeof (name));

				std::string key (name);
				auto itr = _entries.find (key);
				if (itr == _entries.end ()) {
					_entries[name] = new entry (impl_module.release (), std::move (shm));
					loaded = true;
				}
			}
		}
		if (report) {
			report->emplace_back (path, loaded);
		}
	}

	// -----------------------------------------------------------------------------------
	file_system* modules_table::get (const std::string& type) const {
		auto itr = _entries.find (type);
		if (itr == _entries.end ()) {
			return nullptr;
		}
		return itr->second->module ();
	}
	// -----------------------------------------------------------------------------------
	file_system* modules_table::get_single () const {
		ENFORCE(_entries.size() == 1)
		return _entries.begin()->second->module();
	}
} // ns vfs::detail
