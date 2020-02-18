#include <ostream>
#include <iomanip>
#include <vfs/api/mounts.hh>
#include "api/detail/fstab.hh"


namespace vfs
{

	struct mounts::wrapper
	{
		wrapper(core::fstab::entry_map_t::const_iterator it)
			: itr(it)
		{

		}

		core::fstab::entry_map_t::const_iterator itr;
	};

	struct mounts::impl
	{
		impl(core::fstab::entry_map_t::const_iterator beg, core::fstab::entry_map_t::const_iterator en)
			: begin(beg),
			  end(en)
		{

		}
		wrapper begin;
		wrapper end;
	};
	// ===================================================================================
	mounts::mounts(vfs::core::fstab* mtable)
	{
		_impl = std::make_unique<impl>(mtable->_fstab.begin(), mtable->_fstab.end());
	}
	// ----------------------------------------------------------------------------------
	mounts::~mounts() = default;
	// ----------------------------------------------------------------------------------
	mounts::iterator mounts::begin()
	{
		return iterator(_impl->begin);
	}

	// ----------------------------------------------------------------------------------
	mounts::iterator mounts::end()
	{
		return iterator(_impl->end);
	}
	// ===================================================================================
	bool mounts::iterator::_equals(const wrapper& a, const wrapper& b) const noexcept
	{
		return a.itr == b.itr;
	}
	// ----------------------------------------------------------------------------------
	void mounts::iterator::_inc(wrapper& a)
	{
		++a.itr;
	}
	// ===================================================================================
	mounts::data::data(const wrapper& w)
		: _path(w.itr->second.mount_path().to_string()),
		_type(w.itr->second.type()),
		_args(w.itr->second.args()),
		_readonly(w.itr->second.is_readonly())
	{

	}

	// -----------------------------------------------------------------------------------
	std::string mounts::data::type() const noexcept
	{
		return _type;
	}

	// -----------------------------------------------------------------------------------
	std::string mounts::data::args() const noexcept
	{
		return _args;
	}
	// -----------------------------------------------------------------------------------
	std::string mounts::data::path() const noexcept
	{
		return _path;
	}
	// -----------------------------------------------------------------------------------
	bool mounts::data::is_readonly() const noexcept
	{
		return _readonly;
	}
	// ===================================================================================
	mounts::iterator begin(mounts& m)
	{
		return m.begin();
	}
	// ----------------------------------------------------------------------------------
	mounts::iterator end(mounts& m)
	{
		return m.end();
	}
	std::ostream& operator<<(std::ostream& os, const mounts::data& d)
	{
		os << d.type() << "\t" << d.path() << "\t" << d.args() << "\tro: " << std::boolalpha << d.is_readonly();
		return os;
	}
}
