#include "api/detail/path.hh"
#include <functional>

namespace vfs {
	path::path () = default;

	// -----------------------------------------------------------------------------
	path::path (const std::string& pth) {
		_parse_unix (pth);
	}

	// -----------------------------------------------------------------------------
	std::string path::to_string () const {
		return _build_unix ();
	}

	// -----------------------------------------------------------------------------
	path& path::make_directory () {
		push_directory (_name);
		_name.clear ();
		return *this;
	}

	// -----------------------------------------------------------------------------
	path& path::make_file () {
		if (!_dirs.empty () && _name.empty ()) {
			_name = _dirs.back ();
			_dirs.pop_back ();
		}
		return *this;
	}

	// -----------------------------------------------------------------------------
	const std::string& path::operator[] (int n) const {
		if ((size_t)n < _dirs.size ()) {
			return _dirs[n];
		} else {
			return _name;
		}
	}

	// -----------------------------------------------------------------------------
	size_t path::hash () const {
		std::size_t seed = _dirs.size ();
		for (int i = 0; i <= static_cast<int>(_dirs.size ()); i++) {
			const std::string v = (*this)[i];
			if (!v.empty ()) {
				seed ^= std::hash<std::string> () (v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
		}
		return seed;
	}

	// -----------------------------------------------------------------------------
	void path::push_directory (const std::string& dir) {
		if (!dir.empty () && dir != ".") {
			if (dir == "..") {
				if (!_dirs.empty () && _dirs.back () != "..") {
					_dirs.pop_back ();
				}
			} else { _dirs.push_back (dir); }
		}
	}

	// -----------------------------------------------------------------------------
	void path::clear () {
		_name.clear ();
		_dirs.clear ();
	}

	// -----------------------------------------------------------------------------
	void path::_parse_unix (const std::string& path) {
		clear ();

		std::string::const_iterator it = path.begin ();
		std::string::const_iterator end = path.end ();

		if (it != end) {
			if (*it == '/') {
				++it;
			}
			while (it != end) {
				std::string name;
				while (it != end && *it != '/') { name += *it++; }
				if (it != end) {
					if (_dirs.empty ()) {
						push_directory (name);
					} else { push_directory (name); }
				} else { _name = name; }
				if (it != end) { ++it; }
			}
		}
	}

	// -----------------------------------------------------------------------------
	std::string path::_build_unix () const {
		std::string result;

		result.append ("/");

		for (auto x : _dirs) {
			result.append (x);
			result.append ("/");
		}
		result.append (_name);
		return result;
	}

	// -----------------------------------------------------------------------------
	const std::string& path::get_file_name () const {
		return _name;
	}

	// -----------------------------------------------------------------------------
	int path::depth () const {
		return int (_dirs.size ());
	}

} // namespace bsw



