//
// Created by igor on 26/12/2019.
//

#include "modules_table.hh"


namespace vfs::detail
{
	modules_table::entry::entry(vfs_module* obj, std::unique_ptr<shared_module>&& dll)
		: _ref_count(1),
		  _module(obj),
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
	vfs_module* modules_table::entry::module() const
	{
		return _module;
	}
	// ------------------------------------------------------------------------------------
	modules_table::entry::~entry()
	{
		if (_module)
		{
			_module->destructor(_module);
		}
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
	modules_table::iterator modules_table::begin()
	{
		return iterator(_entries.begin());
	}
	// -----------------------------------------------------------------------------------
	modules_table::iterator modules_table::end()
	{
		return iterator(_entries.end());
	}
	// -----------------------------------------------------------------------------------
	vfs_module* modules_table::get(const std::string& type) const
	{
		auto itr = _entries.find(type);
		if (itr == _entries.end())
		{
			return nullptr;
		}
		return itr->second->module();
	}
	// ====================================================================================
	modules_table::iterator::data::data (const map_t::const_iterator& itr)
		: _type (itr->first), _refcount(itr->second->ref_count()), _path(itr->second->path())
	{

	}
	// -----------------------------------------------------------------------------------
	std::string modules_table::iterator::data::type() const
	{
		return _type;
	}
	// -----------------------------------------------------------------------------------
	int modules_table::iterator::data::refcount() const
	{
		return _refcount;
	}
	// -----------------------------------------------------------------------------------
	stdfs::path modules_table::iterator::data::path() const
	{
		return _path;
	}
	// -----------------------------------------------------------------------------------
	modules_table::iterator::data modules_table::iterator::operator*() const
	{
		return data(_value);
	}
	// -----------------------------------------------------------------------------------
	bool modules_table::iterator::operator==(const iterator& other) const
	{
		return _value == other._value;
	}
	// -----------------------------------------------------------------------------------
	bool modules_table::iterator::operator!=(const iterator& other) const
	{
		return !(*this == other);
	}
	// -----------------------------------------------------------------------------------
	modules_table::iterator::data modules_table::iterator::operator++(int)
	{
		data ret(_value);
		(void)++*this;
        return ret;
	}
	// -----------------------------------------------------------------------------------
	modules_table::iterator& modules_table::iterator::operator++()
	{
		++_value;
		return *this;
	}

} // ns vfs::detail
