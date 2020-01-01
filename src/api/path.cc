#include <ostream>
#include "vfs/api/path.hh"

namespace vfs
{
	path::path() = default;
	// -----------------------------------------------------------------------------
	path::path(const std::string& path)
	{
		assign(path);
	}
	// -----------------------------------------------------------------------------
	path::path(const char* path)
	{
		assign(path);
	}
	// -----------------------------------------------------------------------------
	path::path(const path& path) = default;
	// -----------------------------------------------------------------------------
	path::path(const path& parent, const std::string& fileName)
		: _name(parent._name),
		  _dirs(parent._dirs)
	{
		make_directory();
		_name = fileName;
	}
	// -----------------------------------------------------------------------------
	path::path(const path& parent, const char* fileName)
		: _name(parent._name),
		  _dirs(parent._dirs)
	{
		make_directory();
		_name = fileName;
	}
	// -----------------------------------------------------------------------------
	path::~path() = default;
	// -----------------------------------------------------------------------------
	path& path::operator=(const path& path)
	{
		if (this != & path)
		{
			assign(path);
		}
		return *this;
	}
	// -----------------------------------------------------------------------------

	path& path::operator=(const std::string& path)
	{
		assign(path);
		return *this;
	}
	// -----------------------------------------------------------------------------
	path& path::operator=(const char* path)
	{
		assign(path);
		return *this;
	}
	// -----------------------------------------------------------------------------
	void path::swap(path& path)
	{
		std::swap(_name, path._name);
		std::swap(_dirs, path._dirs);
	}
	// -----------------------------------------------------------------------------
	path& path::assign(const path& path)
	{
		if (&path != this)
		{
			_name = path._name;
			_dirs = path._dirs;
		}
		return *this;
	}
	// -----------------------------------------------------------------------------
	path& path::assign(const std::string& path)
	{
		_parse_unix(path);
		return *this;
	}
	// -----------------------------------------------------------------------------
	path& path::assign(const char* path)
	{
		return assign(std::string(path));
	}
	// -----------------------------------------------------------------------------

	std::string path::to_string() const
	{
		return _build_unix();
	}
	// -----------------------------------------------------------------------------
	bool path::try_parse(const std::string& pth)
	{
		try
		{
			path p;
			p.parse(pth);
			assign(p);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	// -----------------------------------------------------------------------------
	path& path::parse_directory(const std::string& path)
	{
		assign(path);
		return make_directory();
	}
	// -----------------------------------------------------------------------------
	path& path::make_directory()
	{
		push_directory(_name);
		_name.clear();
		return *this;
	}
	// -----------------------------------------------------------------------------
	path& path::make_file()
	{
		if (!_dirs.empty() && _name.empty())
		{
			_name = _dirs.back();
			_dirs.pop_back();
		}
		return *this;
	}
	// -----------------------------------------------------------------------------
	path path::parent() const
	{
		path p(*this);
		return p.make_parent();
	}
	// -----------------------------------------------------------------------------
	path& path::make_parent()
	{
		if (_name.empty())
		{
			if (!_dirs.empty())
			{
				if (_dirs.back() == "..")
					_dirs.push_back("..");
				else
					_dirs.pop_back();
			}
		}
		else
		{
			_name.clear();
		}
		return *this;
	}
	// -----------------------------------------------------------------------------
	path& path::append(const path& path)
	{
		make_directory();
		_dirs.insert(_dirs.end(), path._dirs.begin(), path._dirs.end());
		_name = path._name;
		return *this;
	}
	// -----------------------------------------------------------------------------
	bool path::empty() const
	{
		return (_dirs.empty() && _name.empty());
	}
	// -------------------------------------------------------------------------
	const std::string& path::directory(int n) const
	{
		if ((size_t)n < _dirs.size())
			return _dirs[n];
		else
			return _name;
	}
	// -----------------------------------------------------------------------------
	const std::string& path::operator[](int n) const
	{
		if ((size_t)n < _dirs.size())
			return _dirs[n];
		else
			return _name;
	}
	// -----------------------------------------------------------------------------
	void path::push_directory(const std::string& dir)
	{
		if (!dir.empty() && dir != ".")
		{
			if (dir == "..")
			{
				if (!_dirs.empty() && _dirs.back() != "..")
					_dirs.pop_back();
			}
			else _dirs.push_back(dir);
		}
	}
	// -----------------------------------------------------------------------------
	void path::pop_directory()
	{
		_dirs.pop_back();
	}
	// -----------------------------------------------------------------------------
	void path::set_file_name(const std::string& name)
	{
		_name = name;
	}
	// -----------------------------------------------------------------------------
	void path::set_base_name(const std::string& name)
	{
		std::string ext = get_extension();
		_name = name;
		if (!ext.empty())
		{
			_name.append(".");
			_name.append(ext);
		}
	}
	// -----------------------------------------------------------------------------
	std::string path::get_base_name() const
	{
		std::string::size_type pos = _name.rfind('.');
		if (pos != std::string::npos)
			return _name.substr(0, pos);
		else
			return _name;
	}
	// -----------------------------------------------------------------------------
	void path::set_extension(const std::string& extension)
	{
		_name = get_base_name();
		if (!extension.empty())
		{
			_name.append(".");
			_name.append(extension);
		}
	}
	// -----------------------------------------------------------------------------
	std::string path::get_extension() const
	{
		std::string::size_type pos = _name.rfind('.');
		if (pos != std::string::npos)
			return _name.substr(pos + 1);
		else
			return std::string();
	}
	// -----------------------------------------------------------------------------
	void path::clear()
	{
		_name.clear();
		_dirs.clear();
	}
	// -----------------------------------------------------------------------------
	void path::_parse_unix(const std::string& path)
	{
		clear();

		std::string::const_iterator it = path.begin();
		std::string::const_iterator end = path.end();

		if (it != end)
		{
			if (*it == '/')
			{
				++it;
			}
			while (it != end)
			{
				std::string name;
				while (it != end && *it != '/') name += *it++;
				if (it != end)
				{
					if (_dirs.empty())
					{
						push_directory(name);
					}
					else push_directory(name);
				}
				else _name = name;
				if (it != end) ++it;
			}
		}
	}
	// -----------------------------------------------------------------------------
	std::string path::_build_unix() const
	{
		std::string result;

		result.append("/");

		for (auto x : _dirs)
		{
			result.append(x);
			result.append("/");
		}
		result.append(_name);
		return result;
	}
	// -----------------------------------------------------------------------------
	bool path::is_directory() const
	{
		return _name.empty();
	}
	// -----------------------------------------------------------------------------
	bool path::is_file() const
	{
		return !_name.empty();
	}
	// -----------------------------------------------------------------------------
	path& path::parse(const std::string& path)
	{
		return assign(path);
	}
	// -----------------------------------------------------------------------------
	const std::string& path::get_file_name() const
	{
		return _name;
	}
	// -----------------------------------------------------------------------------
	int path::depth() const
	{
		return int(_dirs.size());
	}
	// -----------------------------------------------------------------------------
	path path::for_directory(const std::string& pth)
	{
		path p;
		return p.parse_directory(pth);
	}
	// -----------------------------------------------------------------------------
	void swap(path& p1, path& p2)
	{
		p1.swap(p2);
	}
	// -----------------------------------------------------------------------------
	std::string basename(const path& pth)
	{
		return pth.get_file_name();
	}
	// -----------------------------------------------------------------------------
	path dirname(const path& pth)
	{
		int n = pth.depth();
		path r;
		for (int i = 0; i < n; i++)
		{
			r.push_directory(pth[i]);
		}
		r.make_directory();
		return r;
	}
	// -----------------------------------------------------------------------------
	std::ostream& operator<<(std::ostream& os, const path& pth)
	{
		os << pth.to_string();
		return os;
	}

} // namespace bsw



