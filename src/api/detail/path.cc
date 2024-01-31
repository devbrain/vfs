#include "api/detail/path.hh"
#include <functional>
#include <bsw/exception.hh>

namespace vfs {
	path::path ()
		: m_absolute (false) {
	}

	path::path (bool absolute)
		: m_absolute (absolute) {
	}

	path::path (const std::string& path) {
		assign (path);
	}

	path::path (const std::string& path, style style) {
		assign (path, style);
	}

	path::path (const char* path) {
		ENFORCE(path != nullptr);
		assign (path);
	}

	path::path (const char* path, style style) {
		ENFORCE(path != nullptr);
		assign (path, style);
	}

	path::path (const path& path)
		:
		m_node (path.m_node),
		m_device (path.m_device),
		m_name (path.m_name),
		m_version (path.m_version),
		m_dirs (path.m_dirs),
		m_absolute (path.m_absolute) {
	}

	path::path (path&& path) noexcept
		:
		m_node (std::move (path.m_node)),
		m_device (std::move (path.m_device)),
		m_name (std::move (path.m_name)),
		m_version (std::move (path.m_version)),
		m_dirs (std::move (path.m_dirs)),
		m_absolute (std::move (path.m_absolute)) {
	}

	path::path (const path& parent, const std::string& file_name)
		:
		m_node (parent.m_node),
		m_device (parent.m_device),
		m_name (parent.m_name),
		m_version (parent.m_version),
		m_dirs (parent.m_dirs),
		m_absolute (parent.m_absolute) {
		make_directory ();
		m_name = file_name;
	}

	path::path (const path& parent, const char* file_name)
		:
		m_node (parent.m_node),
		m_device (parent.m_device),
		m_name (parent.m_name),
		m_version (parent.m_version),
		m_dirs (parent.m_dirs),
		m_absolute (parent.m_absolute) {
		make_directory ();
		m_name = file_name;
	}

	path::path (const path& parent, const path& relative)
		:
		m_node (parent.m_node),
		m_device (parent.m_device),
		m_name (parent.m_name),
		m_version (parent.m_version),
		m_dirs (parent.m_dirs),
		m_absolute (parent.m_absolute) {
		resolve (relative);
	}

	path::~path () = default;

	path& path::operator= (const path& pth) {
		return assign (pth);
	}

	path& path::operator= (path&& pth) noexcept {
		m_node = std::move (pth.m_node);
		m_device = std::move (pth.m_device);
		m_name = std::move (pth.m_name);
		m_version = std::move (pth.m_version);
		m_dirs = std::move (pth.m_dirs);
		m_absolute = pth.m_absolute;
		return *this;
	}

	path& path::operator= (const std::string& pth) {
		return assign (pth);
	}

	path& path::operator= (const char* pth) {
		ENFORCE(pth != nullptr);
		return assign (pth);
	}

	void path::swap (path& pth) {
		std::swap (m_node, pth.m_node);
		std::swap (m_device, pth.m_device);
		std::swap (m_name, pth.m_name);
		std::swap (m_version, pth.m_version);
		std::swap (m_dirs, pth.m_dirs);
		std::swap (m_absolute, pth.m_absolute);
	}

	path& path::assign (const path& pth) {
		if (&pth != this) {
			m_node = pth.m_node;
			m_device = pth.m_device;
			m_name = pth.m_name;
			m_version = pth.m_version;
			m_dirs = pth.m_dirs;
			m_absolute = pth.m_absolute;
		}
		return *this;
	}

	path& path::assign (const std::string& pth) {
#if defined(PREDEF_OS_WINDOWS)
		parseWindows(path);
#else
		parse_unix (pth);
#endif
		return *this;
	}

	path& path::assign (const std::string& pth, style style) {
		switch (style) {
			case PATH_UNIX: parse_unix (pth);
				break;
			case PATH_WINDOWS: parse_windows (pth);
				break;
			case PATH_VMS: parse_vms (pth);
				break;
			case PATH_NATIVE: assign (pth);
				break;
			case PATH_GUESS: parse_guess (pth);
				break;
			default: ENFORCE(false)
		}
		return *this;
	}

	path& path::assign (const char* pth) {
		return assign (std::string (pth));
	}

	std::string path::to_string () const {
#if defined(PREDEF_OS_WINDOWS)
		return buildWindows();
#else
		return build_unix ();
#endif
	}

	std::string path::to_string (style style) const {
		switch (style) {
			case PATH_UNIX: return build_unix ();
			case PATH_WINDOWS: return build_windows ();
			case PATH_VMS: return build_vms ();
			case PATH_NATIVE:
			case PATH_GUESS: return to_string ();
			default: ENFORCE(false);
		}
		return {};
	}

	bool path::try_parse (const std::string& pth) {
		try {
			path p;
			p.parse (pth);
			assign (p);
			return true;
		}
		catch (...) {
			return false;
		}
	}

	bool path::try_parse (const std::string& pth, style style) {
		try {
			path p;
			p.parse (pth, style);
			assign (p);
			return true;
		}
		catch (...) {
			return false;
		}
	}

	path& path::parse_directory (const std::string& pth) {
		assign (pth);
		return make_directory ();
	}

	path& path::parse_directory (const std::string& pth, style style) {
		assign (pth, style);
		return make_directory ();
	}

	path& path::make_directory () {
		push_directory (m_name);
		m_name.clear ();
		m_version.clear ();
		return *this;
	}

	path& path::make_file () {
		if (!m_dirs.empty () && m_name.empty ()) {
			m_name = m_dirs.back ();
			m_dirs.pop_back ();
		}
		return *this;
	}

	path& path::make_absolute (const path& base) {
		if (!m_absolute) {
			path tmp = base;
			tmp.make_directory ();
			for (const auto& d : m_dirs) {
				tmp.push_directory (d);
			}
			m_node = tmp.m_node;
			m_device = tmp.m_device;
			m_dirs = tmp.m_dirs;
			m_absolute = base.m_absolute;
		}
		return *this;
	}

	path path::absolute (const path& base) const {
		path result (*this);
		if (!result.m_absolute) {
			result.make_absolute (base);
		}
		return result;
	}

	path path::parent () const {
		path p (*this);
		return p.make_parent ();
	}

	path& path::make_parent () {
		if (m_name.empty ()) {
			if (m_dirs.empty ()) {
				if (!m_absolute) {
					m_dirs.emplace_back ("..");
				}
			} else {
				if (m_dirs.back () == "..") {
					m_dirs.emplace_back ("..");
				} else {
					m_dirs.pop_back ();
				}
			}
		} else {
			m_name.clear ();
			m_version.clear ();
		}
		return *this;
	}

	path& path::append (const path& pth) {
		make_directory ();
		m_dirs.insert (m_dirs.end (), pth.m_dirs.begin (), pth.m_dirs.end ());
		m_name = pth.m_name;
		m_version = pth.m_version;
		return *this;
	}

	path& path::resolve (const path& pth) {
		if (pth.is_absolute ()) {
			assign (pth);
		} else {
			for (int i = 0; i < pth.depth (); ++i) {
				push_directory (pth[i]);
			}
			m_name = pth.m_name;
		}
		return *this;
	}

	path& path::set_node (const std::string& node) {
		m_node = node;
		m_absolute = m_absolute || !node.empty ();
		return *this;
	}

	path& path::set_device (const std::string& device) {
		m_device = device;
		m_absolute = m_absolute || !device.empty ();
		return *this;
	}

	const std::string& path::directory (int n) const {
		ENFORCE (0 <= n && n <= (int)m_dirs.size ());

		if (n < (int)m_dirs.size ()) {
			return m_dirs[n];
		} else {
			return m_name;
		}
	}

	const std::string& path::operator[] (int n) const {
		ENFORCE (0 <= n && n <= (int)m_dirs.size ());

		if (n < (int)m_dirs.size ()) {
			return m_dirs[n];
		} else {
			return m_name;
		}
	}

	path& path::push_directory (const std::string& dir) {
		if (!dir.empty () && dir != ".") {
			if (dir == "..") {
				if (!m_dirs.empty () && m_dirs.back () != "..") {
					m_dirs.pop_back ();
				} else if (!m_absolute) {
					m_dirs.push_back (dir);
				}
			} else { m_dirs.push_back (dir); }
		}
		return *this;
	}

	path& path::pop_directory () {
		ENFORCE (!m_dirs.empty ());

		m_dirs.pop_back ();
		return *this;
	}

	path& path::pop_front_directory () {
		ENFORCE (!m_dirs.empty ());

		auto it = m_dirs.begin ();
		m_dirs.erase (it);
		return *this;
	}

	path& path::set_file_name (const std::string& name) {
		m_name = name;
		return *this;
	}

	path& path::set_base_name (const std::string& name) {
		std::string ext = get_extension ();
		m_name = name;
		if (!ext.empty ()) {
			m_name.append (".");
			m_name.append (ext);
		}
		return *this;
	}

	std::string path::get_base_name () const {
		std::string::size_type pos = m_name.rfind ('.');
		if (pos != std::string::npos) {
			return m_name.substr (0, pos);
		} else {
			return m_name;
		}
	}

	path& path::set_extension (const std::string& extension) {
		m_name = get_base_name ();
		if (!extension.empty ()) {
			m_name.append (".");
			m_name.append (extension);
		}
		return *this;
	}

	std::string path::get_extension () const {
		std::string::size_type pos = m_name.rfind ('.');
		if (pos != std::string::npos) {
			return m_name.substr (pos + 1);
		} else {
			return {};
		}
	}

	path& path::clear () {
		m_node.clear ();
		m_device.clear ();
		m_name.clear ();
		m_dirs.clear ();
		m_version.clear ();
		m_absolute = false;
		return *this;
	}

	void path::parse_unix (const std::string& pth) {
		clear ();

		std::string::const_iterator it = pth.begin ();
		std::string::const_iterator end = pth.end ();

		if (it != end) {
			if (*it == '/') {
				m_absolute = true;
				++it;
			}

			while (it != end) {
				std::string name;
				while (it != end && *it != '/') { name += *it++; }
				if (it != end) {
					if (m_dirs.empty ()) {
						if (!name.empty () && *(name.rbegin ()) == ':') {
							m_absolute = true;
							m_device.assign (name, 0, name.length () - 1);
						} else {
							push_directory (name);
						}
					} else { push_directory (name); }
				} else { m_name = name; }
				if (it != end) { ++it; }
			}
		}
	}

	void path::parse_windows (const std::string& pth) {
		clear ();

		std::string::const_iterator it = pth.begin ();
		std::string::const_iterator end = pth.end ();

		if (it != end) {
			if (*it == '\\' || *it == '/') {
				m_absolute = true;
				++it;
			}
			if (m_absolute && it != end && (*it == '\\' || *it == '/')) // UNC
			{
				++it;
				while (it != end && *it != '\\' && *it != '/') { m_node += *it++; }
				if (it != end) { ++it; }
			} else if (it != end) {
				char d = *it++;
				if (it != end && *it == ':') // drive letter
				{
					if (m_absolute || !((d >= 'a' && d <= 'z') || (d >= 'A' && d <= 'Z'))) {
						RAISE_EX("Bad path syntax ", pth);
					}
					m_absolute = true;
					m_device += d;
					++it;
					if (it == end || (*it != '\\' && *it != '/')) { RAISE_EX("Bad path syntax ", pth); }
					++it;
				} else { --it; }
			}
			while (it != end) {
				std::string name;
				while (it != end && *it != '\\' && *it != '/') { name += *it++; }
				if (it != end) {
					push_directory (name);
				} else {
					m_name = name;
				}
				if (it != end) { ++it; }
			}
		}
		if (!m_node.empty () && m_dirs.empty () && !m_name.empty ()) {
			make_directory ();
		}
	}

	void path::parse_vms (const std::string& pth) {
		clear ();

		std::string::const_iterator it = pth.begin ();
		std::string::const_iterator end = pth.end ();

		if (it != end) {
			std::string name;
			while (it != end && *it != ':' && *it != '[' && *it != ';') { name += *it++; }
			if (it != end) {
				if (*it == ':') {
					++it;
					if (it != end && *it == ':') {
						m_node = name;
						++it;
					} else { m_device = name; }
					m_absolute = true;
					name.clear ();
				}
				if (it != end) {
					if (m_device.empty () && *it != '[') {
						while (it != end && *it != ':' && *it != ';') { name += *it++; }
						if (it != end) {
							if (*it == ':') {
								m_device = name;
								m_absolute = true;
								name.clear ();
								++it;
							}
						}
					}
				}
				if (name.empty ()) {
					if (it != end && *it == '[') {
						++it;
						if (it != end) {
							m_absolute = true;
							if (*it == '.') {
								m_absolute = false;
								++it;
							} else if (*it == ']' || *it == '-') {
								m_absolute = false;
							}
							while (it != end && *it != ']') {
								name.clear ();
								if (*it == '-') {
									name = "-";
								} else {
									while (it != end && *it != '.' && *it != ']') { name += *it++; }
								}
								if (!name.empty ()) {
									if (name == "-") {
										if (m_dirs.empty () || m_dirs.back () == "..") {
											m_dirs.emplace_back ("..");
										} else {
											m_dirs.pop_back ();
										}
									} else { m_dirs.push_back (name); }
								}
								if (it != end && *it != ']') { ++it; }
							}
							if (it == end) { RAISE_EX("Bad path syntax ", pth); }
							++it;
							if (it != end && *it == '[') {
								if (!m_absolute) { RAISE_EX("Bad path syntax ", pth); }
								++it;
								if (it != end && *it == '.') { RAISE_EX("Bad path syntax ", pth); }
								int d = int (m_dirs.size ());
								while (it != end && *it != ']') {
									name.clear ();
									if (*it == '-') {
										name = "-";
									} else {
										while (it != end && *it != '.' && *it != ']') { name += *it++; }
									}
									if (!name.empty ()) {
										if (name == "-") {
											if ((int)m_dirs.size () > d) {
												m_dirs.pop_back ();
											}
										} else { m_dirs.push_back (name); }
									}
									if (it != end && *it != ']') { ++it; }
								}
								if (it == end) { RAISE_EX("Bad path syntax ", pth); }
								++it;
							}
						}
						m_name.clear ();
					}
					while (it != end && *it != ';') { m_name += *it++; }
				} else { m_name = name; }
				if (it != end && *it == ';') {
					++it;
					while (it != end) { m_version += *it++; }
				}
			} else { m_name = name; }
		}
	}

	void path::parse_guess (const std::string& pth) {
		bool hasBackslash = false;
		bool hasSlash = false;
		bool hasOpenBracket = false;
		bool hasClosBracket = false;
		bool isWindows = pth.length () > 2 && pth[1] == ':' && (pth[2] == '/' || pth[2] == '\\');
		std::string::const_iterator end = pth.end ();
		std::string::const_iterator semiIt = end;
		if (!isWindows) {
			for (std::string::const_iterator it = pth.begin (); it != end; ++it) {
				switch (*it) {
					case '\\': hasBackslash = true;
						break;
					case '/': hasSlash = true;
						break;
					case '[': hasOpenBracket = true;
					case ']': hasClosBracket = hasOpenBracket;
					case ';': semiIt = it;
						break;
				}
			}
		}
		if (hasBackslash || isWindows) {
			parse_windows (pth);
		} else if (hasSlash) {
			parse_unix (pth);
		} else {
			bool isVMS = hasClosBracket;
			if (!isVMS && semiIt != end) {
				isVMS = true;
				++semiIt;
				while (semiIt != end) {
					if (*semiIt < '0' || *semiIt > '9') {
						isVMS = false;
						break;
					}
					++semiIt;
				}
			}
			if (isVMS) {
				parse_vms (pth);
			} else {
				parse_unix (pth);
			}
		}
	}

	std::string path::build_unix () const {
		std::string result;
		if (!m_device.empty ()) {
			result.append ("/");
			result.append (m_device);
			result.append (":/");
		} else if (m_absolute) {
			result.append ("/");
		}
		for (const auto& d : m_dirs) {
			result.append (d);
			result.append ("/");
		}
		result.append (m_name);
		return result;
	}

	std::string path::build_windows () const {
		std::string result;
		if (!m_node.empty ()) {
			result.append ("\\\\");
			result.append (m_node);
			result.append ("\\");
		} else if (!m_device.empty ()) {
			result.append (m_device);
			result.append (":\\");
		} else if (m_absolute) {
			result.append ("\\");
		}
		for (const auto& d : m_dirs) {
			result.append (d);
			result.append ("\\");
		}
		result.append (m_name);
		return result;
	}

	std::string path::build_vms () const {
		std::string result;
		if (!m_node.empty ()) {
			result.append (m_node);
			result.append ("::");
		}
		if (!m_device.empty ()) {
			result.append (m_device);
			result.append (":");
		}
		if (!m_dirs.empty ()) {
			result.append ("[");
			if (!m_absolute && m_dirs[0] != "..") {
				result.append (".");
			}
			for (auto it = m_dirs.begin (); it != m_dirs.end (); ++it) {
				if (it != m_dirs.begin () && *it != "..") {
					result.append (".");
				}
				if (*it == "..") {
					result.append ("-");
				} else {
					result.append (*it);
				}
			}
			result.append ("]");
		}
		result.append (m_name);
		if (!m_version.empty ()) {
			result.append (";");
			result.append (m_version);
		}
		return result;
	}

	size_t path::hash () const {
		std::size_t seed = m_dirs.size ();
		for (int i = 0; i <= static_cast<int>(m_dirs.size ()); i++) {
			const std::string v = (*this)[i];
			if (!v.empty ()) {
				seed ^= std::hash<std::string> () (v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
		}
		return seed;
	}
} // namespace bsw
#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#include <doctest/doctest.h>


#include <stdexcept>
#include <doctest/doctest.h>

VFS_API void from_dll();   // to silence "-Wmissing-declarations" with GCC
VFS_API void from_dll() {} // force the creation of a .lib file with MSVC

using namespace vfs;

TEST_SUITE("path test") {
	TEST_CASE("parse unix") {
		path p;
		p.parse ("", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "");

		p.parse ("/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/");

		p.parse ("/usr", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr");

		p.parse ("/usr/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/");

		p.parse ("usr/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/");

		p.parse ("usr", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p[0] == "usr");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr");

		p.parse ("/usr/local", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local");
	}

	TEST_CASE("parse unix 2") {
		path p;
		p.parse ("/usr/local/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/");

		p.parse ("usr/local/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local/");

		p.parse ("usr/local", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local");

		p.parse ("/usr/local/bin", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin");

		p.parse ("/usr/local/bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");
	}

	TEST_CASE("parse unix 3") {
		path p;
		p.parse ("//usr/local/bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr//local/bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local//bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local/bin//", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local/./bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("./usr/local/bin/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local/bin/");

		p.parse ("./usr/local/bin/./", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local/bin/");

		p.parse ("./usr/local/bin/.", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local/bin/.");
	}

	TEST_CASE("parse unix 4") {
		path p;
		p.parse ("/usr/local/lib/../bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local/lib/../bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local/lib/../../", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/");

		p.parse ("/usr/local/lib/..", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "lib");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/lib/..");

		p.parse ("../usr/local/lib/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 4);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "usr");
		REQUIRE (p[2] == "local");
		REQUIRE (p[3] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "../usr/local/lib/");

		p.parse ("/usr/../lib/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/lib/");

		p.parse ("/usr/../../lib/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/lib/");

		p.parse ("local/../../lib/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "../lib/");

		p.parse ("a/b/c/d", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "a");
		REQUIRE (p[1] == "b");
		REQUIRE (p[2] == "c");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "a/b/c/d");
	}

	TEST_CASE("parse unix 5") {
		path p;
		p.parse ("/c:/windows/system32/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.get_device () == "c");
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "windows");
		REQUIRE (p[1] == "system32");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/c:/windows/system32/");
	}

	TEST_CASE("parse windows") {
		path p;
		p.parse ("", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "");

		p.parse ("/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\");

		p.parse ("\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\");

		p.parse ("/usr", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr");

		p.parse ("\\usr", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr");

		p.parse ("/usr/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\");

		p.parse ("\\usr\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\");
	}

	TEST_CASE("parse windows 2") {
		path p;
		p.parse ("usr/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\");

		p.parse ("usr", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p[0] == "usr");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr");

		p.parse ("usr\\", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\");

		p.parse ("/usr/local", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local");

		p.parse ("\\usr\\local", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local");

		p.parse ("/usr/local/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\");

		p.parse ("usr/local/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local\\");

		p.parse ("usr/local", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local");

		p.parse ("/usr/local/bin", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin");

		p.parse ("/usr/local/bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr//local/bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr/local//bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr/local/bin//", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");
	}

	TEST_CASE("parse windows 3") {
		path p;
		p.parse ("/usr/local/./bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("./usr/local/bin/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local\\bin\\");

		p.parse ("./usr/local/bin/./", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local\\bin\\");

		p.parse ("./usr/local/bin/.", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local\\bin\\.");

		p.parse ("/usr/local/lib/../bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr/local/lib/../bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("\\usr\\local\\lib\\..\\bin\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr/local/lib/../../", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\");

		p.parse ("/usr/local/lib/..", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "lib");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\lib\\..");

		p.parse ("../usr/local/lib/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 4);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "usr");
		REQUIRE (p[2] == "local");
		REQUIRE (p[3] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "..\\usr\\local\\lib\\");

		p.parse ("/usr/../lib/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\lib\\");

		p.parse ("/usr/../../lib/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\lib\\");

		p.parse ("local/../../lib/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "..\\lib\\");
	}

	TEST_CASE("parse windows 4") {
		path p;
		p.parse ("\\\\server\\files", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "files");
		REQUIRE (p.get_node () == "server");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\");

		p.parse ("\\\\server\\files\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "files");
		REQUIRE (p.get_node () == "server");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\");

		p.parse ("\\\\server\\files\\file", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "files");
		REQUIRE (p.get_node () == "server");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\file");

		p.parse ("\\\\server\\files\\dir\\file", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "files");
		REQUIRE (p[1] == "dir");
		REQUIRE (p.get_node () == "server");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\dir\\file");

		p.parse ("\\\\server\\files\\dir\\file", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "files");
		REQUIRE (p[1] == "dir");
		REQUIRE (p.get_node () == "server");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\dir\\file");

		p.parse ("\\\\server", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_node () == "server");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\");

		p.parse ("c:\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "c");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\");

		p.parse ("c:\\WinNT", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "c");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\WinNT");

		p.parse ("c:\\WinNT\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "WinNT");
		REQUIRE (p.get_device () == "c");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\WinNT\\");

		CHECK_THROWS_AS(p.parse ("~:\\", path::PATH_WINDOWS), std::exception);
		CHECK_THROWS_AS(p.parse ("c:file.txt", path::PATH_WINDOWS), std::exception);

		p.parse ("a\\b\\c\\d", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "a");
		REQUIRE (p[1] == "b");
		REQUIRE (p[2] == "c");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "a\\b\\c\\d");
	}

	TEST_CASE("parse vms") {
		path p;
		p.parse ("", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "");

		p.parse ("[]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "");

		p.parse ("[foo]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[.foo]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[.foo]");

		p.parse ("[foo.bar]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar]");

		p.parse ("[.foo.bar]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[.foo.bar]");

		p.parse ("[foo.bar.foobar]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p[2] == "foobar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar.foobar]");

		p.parse ("[.foo.bar.foobar]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p[2] == "foobar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[.foo.bar.foobar]");
	}

	TEST_CASE("parse vms2") {
		path p;
		p.parse ("[foo][bar]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar]");

		p.parse ("[foo.][bar]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar]");

		p.parse ("[foo.bar][foo]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p[2] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar.foo]");

		CHECK_THROWS_AS(p.parse ("[foo.bar][.foo]", path::PATH_VMS), std::exception);
		CHECK_THROWS_AS(p.parse ("[.foo.bar][foo]", path::PATH_VMS), std::exception);

		p.parse ("[-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[-]");

		p.parse ("[--]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[--]");

		p.parse ("[---]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");

		p.parse ("[-.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[--]");

		p.parse ("[.-.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[--]");

		p.parse ("[-.-.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");

		p.parse ("[.-.-.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");

		p.parse ("[.--.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");

		p.parse ("[--.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");
	}

	TEST_CASE("parse vms 3") {
		path p;
		p.parse ("[foo][-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][--]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][-.-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][bar.-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][bar.foo.-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar]");

		p.parse ("[foo][bar.foo.--]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][bar.foo.---]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][bar.foo.-.-.-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");
	}

	TEST_CASE("parse guess") {
		path p;

		p.parse ("foo:bar.txt;5", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "foo");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());

		REQUIRE (p.to_string (path::PATH_VMS) == "foo:bar.txt;5");

		p.parse ("/usr/local/bin", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin");

		p.parse ("\\\\server\\files", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "files");
		REQUIRE (p.get_node () == "server");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\");

		p.parse ("c:\\WinNT", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "c");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\WinNT");

		p.parse ("foo:bar.txt;5", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "foo");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "foo:bar.txt;5");

		p.parse ("[foo]bar.txt", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]bar.txt");
	}

	TEST_CASE("test basename ext") {
		path p ("foo.bar");
		REQUIRE (p.get_file_name () == "foo.bar");
		REQUIRE (p.get_base_name () == "foo");
		REQUIRE (p.get_extension () == "bar");

		p.set_base_name ("readme");
		REQUIRE (p.get_file_name () == "readme.bar");
		REQUIRE (p.get_base_name () == "readme");
		REQUIRE (p.get_extension () == "bar");

		p.set_extension ("txt");
		REQUIRE (p.get_file_name () == "readme.txt");
		REQUIRE (p.get_base_name () == "readme");
		REQUIRE (p.get_extension () == "txt");

		p.set_extension ("html");
		REQUIRE (p.get_file_name () == "readme.html");
		REQUIRE (p.get_base_name () == "readme");
		REQUIRE (p.get_extension () == "html");

		p.set_base_name ("index");
		REQUIRE (p.get_file_name () == "index.html");
		REQUIRE (p.get_base_name () == "index");
		REQUIRE (p.get_extension () == "html");
	}

	TEST_CASE("test asolute") {
		path base ("C:\\Program Files\\", path::PATH_WINDOWS);
		path rel ("Poco");
		path abs = rel.absolute (base);
		REQUIRE (abs.to_string (path::PATH_WINDOWS) == "C:\\Program Files\\Poco");

		base.parse ("/usr/local", path::PATH_UNIX);
		rel.parse ("Poco/include", path::PATH_UNIX);
		abs = rel.absolute (base);
		REQUIRE (abs.to_string (path::PATH_UNIX) == "/usr/local/Poco/include");

		base.parse ("/usr/local/bin", path::PATH_UNIX);
		rel.parse ("../Poco/include", path::PATH_UNIX);
		abs = rel.absolute (base);
		REQUIRE (abs.to_string (path::PATH_UNIX) == "/usr/local/Poco/include");
	}

	TEST_CASE("test parent") {
		path p ("/usr/local/include", path::PATH_UNIX);
		p.make_parent ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/");
		p.make_parent ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/");
		p.make_parent ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "/");
		p.make_parent ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "/");
	}

	TEST_CASE("test directory") {
		path p = path ("/usr/local/include", path::PATH_UNIX);
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/include");

		p = path ("/usr/local/include/", path::PATH_UNIX);
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/include/");
	}

	TEST_CASE("test swap") {
		path p1 ("c:\\temp\\foo.bar");
		path p2 ("\\\\server\\files\\foo.bar");
		p1.swap (p2);
		REQUIRE (p1.to_string () == "\\\\server\\files\\foo.bar");
		REQUIRE (p2.to_string () == "c:\\temp\\foo.bar");
	}

	TEST_CASE("test resolve") {
		path p ("c:\\foo\\", path::PATH_WINDOWS);
		p.resolve ("test.dat");
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\foo\\test.dat");

		p.assign ("c:\\foo\\", path::PATH_WINDOWS);
		p.resolve (path ("d:\\bar.txt", path::PATH_WINDOWS));
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "d:\\bar.txt");

		p.assign ("c:\\foo\\bar.txt", path::PATH_WINDOWS);
		p.resolve ("foo.txt");
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\foo\\foo.txt");

		p.assign (R"(c:\foo\bar\)", path::PATH_WINDOWS);
		p.resolve (path ("..\\baz\\test.dat", path::PATH_WINDOWS));
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\foo\\baz\\test.dat");
	}

	TEST_CASE("test push pop") {
		path p;
		p.push_directory ("a");
		p.push_directory ("b");
		p.push_directory ("c");
		REQUIRE (p.to_string (path::PATH_UNIX) == "a/b/c/");

		p.pop_directory ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "a/b/");

		p.pop_front_directory ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "b/");
	}
}
