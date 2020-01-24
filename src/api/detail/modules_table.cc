//
// Created by igor on 26/12/2019.
//

#include "modules_table.hh"


namespace vfs::core
{
	modules_table::entry::entry(vfs_module* obj, std::unique_ptr<shared_module>&& dll)
		: _ref_count(1),
		  _fs(new filesystem(obj)),
		  _dll(std::move(dll))
	{

	}
	// ------------------------------------------------------------------------------------
	void modules_table::entry::inc_ref()
	{
		_ref_count++;
	}
	// ------------------------------------------------------------------------------------
	bool modules_table::entry::dec_ref()
	{
		_ref_count--;
		if (_ref_count == 0)
		{
			delete this;
			return false;
		}
		return true;
	}
	// ------------------------------------------------------------------------------------
	stdfs::path modules_table::entry::path() const
	{
		return _dll->path();
	}
	// ------------------------------------------------------------------------------------
	int modules_table::entry::ref_count() const
	{
		return _ref_count;
	}
	// ------------------------------------------------------------------------------------
	filesystem* modules_table::entry::module() const
	{
		return _fs;
	}
	// ------------------------------------------------------------------------------------
	modules_table::entry::~entry()
	{
		delete _fs;
	}
	// ===================================================================================
	modules_table::modules_table(const stdfs::path& path)
	{
		add(path);
	}
	// -----------------------------------------------------------------------------------
	modules_table::~modules_table()
	{
		for (auto e : _entries)
		{
			delete e.second;
		}
	}
	// -----------------------------------------------------------------------------------
	void modules_table::add(const stdfs::path& path)
	{
		if (stdfs::is_directory(path))
		{
			for (auto& p: stdfs::directory_iterator(path))
			{
				add(p);
			}
		}
		else
		{
			if (stdfs::is_regular_file(path))
			{
#if defined(_WIN32) || defined(WIN32)
#define DLL_EXT  L".dll"
				using ext_type = std::wstring;
#else
#define DLL_EXT  ".so"
				using ext_type = std::string;
#endif
				ext_type ext = path.extension().c_str();
				ext_type xxx = path.c_str();
				if (ext == DLL_EXT)
				{
					_add_file(path);
				}
			}
		}
	}
	// -----------------------------------------------------------------------------------
	void modules_table::_add_file(const stdfs::path& path)
	{
		std::unique_ptr<shared_module> shm = shared_module::load(path);
		if (shm)
		{
			auto creator = shm->get();

			std::unique_ptr<vfs_module> impl_module = std::make_unique<vfs_module>();

			creator(impl_module.get());

			if (impl_module->get_name)
			{
				char name[128] = { 0 };
				impl_module->get_name(impl_module->opaque, name, sizeof(name));
				std::string key(name);

				auto itr = _entries.find(key);
				if (itr == _entries.end())
				{
					_entries[name] = new entry(impl_module.release(), std::move(shm));
				}
			}
		}
	}
	// -----------------------------------------------------------------------------------
	filesystem* modules_table::get(const std::string& type) const
	{
		auto itr = _entries.find(type);
		if (itr == _entries.end())
		{
			return nullptr;
		}
		return itr->second->module();
	}
	// ====================================================================================


} // ns vfs::detail