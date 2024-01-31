#include <ostream>
#include "vfs/modules.hh"
#include "api/detail/modules_table.hh"

namespace vfs {

	struct modules::wrapper {
		explicit wrapper (core::modules_table::map_t::const_iterator it)
			: itr (it) {

		}

		core::modules_table::map_t::const_iterator itr;
	};

	struct modules::impl {
		impl (core::modules_table::map_t::const_iterator beg, core::modules_table::map_t::const_iterator en)
			: begin (beg),
			  end (en) {

		}

		wrapper begin;
		wrapper end;
	};

	// ===================================================================================
	modules::modules (vfs::core::modules_table* mtable) {
		_impl = std::make_unique<impl> (mtable->_entries.begin (), mtable->_entries.end ());
	}

	// ----------------------------------------------------------------------------------
	modules::~modules () = default;

	// ----------------------------------------------------------------------------------
	modules::iterator modules::begin () {
		return iterator (_impl->begin);
	}

	// ----------------------------------------------------------------------------------
	modules::iterator modules::end () {
		return iterator (_impl->end);
	}

	// ===================================================================================
	bool modules::iterator::_equals (const wrapper& a, const wrapper& b) const noexcept {
		return a.itr == b.itr;
	}

	// ----------------------------------------------------------------------------------
	void modules::iterator::_inc (wrapper& a) {
		++a.itr;
	}

	// ===================================================================================
	modules::data::data (const wrapper& w)
		: _type (w.itr->first), _refcount (w.itr->second->ref_count ()), _path (w.itr->second->path ()) {

	}

	// -----------------------------------------------------------------------------------
	std::string modules::data::type () const noexcept {
		return _type;
	}

	// -----------------------------------------------------------------------------------
	int modules::data::refcount () const noexcept {
		return _refcount;
	}

	// -----------------------------------------------------------------------------------
	std::filesystem::path modules::data::path () const noexcept {
		return _path;
	}

	// ===================================================================================
	modules::iterator begin (modules& m) {
		return m.begin ();
	}

	// ----------------------------------------------------------------------------------
	modules::iterator end (modules& m) {
		return m.end ();
	}

	std::ostream& operator<< (std::ostream& os, const modules::data& d) {
		os << d.type () << "\t" << d.path ();
		return os;
	}
}