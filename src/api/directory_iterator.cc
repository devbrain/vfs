#include <vfs/api/directory_iterator.hh>
#include <vfs/api/exception.hh>
#include "api/detail/file_system.hh"
#include "api/detail/stats_converter.hh"
#include <bsw/errors.hh>


namespace vfs
{
	struct directory::impl
	{
		impl(std::shared_ptr<core::inode> ino)
		: di (ino->get_directory_iterator()),
		  node(ino)
		{
			if (!di)
			{
			    THROW_EXCEPTION_EX(vfs::exception, "failed to obtain directory iterator");
			}
		}
		std::unique_ptr<core::directory_iterator> di;
		std::shared_ptr<core::inode> node;
	};
	// -----------------------------------------------------------------------------------------
	std::tuple<std::string, stats> directory::next ()
	{
		std::string name = _pimpl->di->next();
		core::stats st;
		auto child = _pimpl->node->lookup(name);
		if (child)
        {
            child->stat(st);
            return std::make_tuple(name, convert(st));
        }
		throw std::runtime_error(name + " was removed during iteration");
	}
	// -----------------------------------------------------------------------------------------
	bool directory::has_next() const
	{
		return _pimpl->di->has_next();
	}
	// -----------------------------------------------------------------------------------------
	directory::~directory() = default;
	// -----------------------------------------------------------------------------------------
	directory::directory(std::shared_ptr<core::inode> ino)
	{
		_pimpl = std::make_unique<impl>(ino);
	}
	// ===================================================================================
	directory_iterator::directory_iterator()
		: _directory(nullptr), _ino_stats(value_type())
	{
	}
	//-----------------------------------------------------------------------------------
	directory_iterator::directory_iterator(directory& directoryToWrap)
		: directory_iterator()
	{
		if (directoryToWrap.has_next())
		{
			_directory = &directoryToWrap;
			_ino_stats = directoryToWrap.next();
		}
	}
	//-----------------------------------------------------------------------------------
	directory_iterator directory_iterator::operator = (directory_iterator& other)
	{
		if (this != &other)
		{
			_directory = other._directory;
			_ino_stats = other._ino_stats;
		}
		return *this;
	}
	//-----------------------------------------------------------------------------------
	directory_iterator::directory_iterator(const directory_iterator& other)
		:_directory(other._directory),
		_ino_stats(other._ino_stats)
	{
	}
	//-----------------------------------------------------------------------------------
	directory_iterator::reference directory_iterator::operator*() const
	{
		assert_directory();
		return _ino_stats;
	}
	//-----------------------------------------------------------------------------------
	directory_iterator::pointer directory_iterator::operator->() const
	{
		return &(operator*());
	}
	//-----------------------------------------------------------------------------------
	directory_iterator& directory_iterator::operator++()
	{
		assert_directory();
		if (_directory->has_next())
		{
			_ino_stats = _directory->next();
		}
		else
		{
		    // invalidate
		    this->_directory = nullptr;
		    this->_ino_stats = value_type {};
		}
		return *this;
	}
	//-----------------------------------------------------------------------------------
	directory_iterator directory_iterator::operator++(int)
	{
		directory_iterator previousState = *this;
		operator++();
		return previousState;
	}
	//-----------------------------------------------------------------------------------
	bool operator==(const directory_iterator& lhs, const directory_iterator& rhs)
	{
		return lhs._directory == rhs._directory;
	}
	//-----------------------------------------------------------------------------------
	bool operator!=(const directory_iterator& lhs, const directory_iterator& rhs)
	{
		return !(lhs == rhs);
	}
	//-----------------------------------------------------------------------------------
	void directory_iterator::assert_directory() const
	{
		if (!_directory)
		{
			throw std::out_of_range("iterator is out of range");
		}
	}
	// ===================================================================================
	directory_iterator begin(directory& w)
	{
		return directory_iterator(w);
	}
	// -----------------------------------------------------------------------------------
	directory_iterator end(directory& )
	{
		return directory_iterator();
	}
}