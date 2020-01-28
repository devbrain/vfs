#include "filesystem.hh"

namespace vfs::core
{
	filesystem::filesystem(vfs_module* ops)
	: _module(ops)
	{
	}
	// ---------------------------------------------------------------------
	filesystem::~filesystem()
	{
		_module->destructor(_module);
	}
	// ---------------------------------------------------------------------
	std::unique_ptr<inode> filesystem::load_root(const std::string& params)
	{
		return std::unique_ptr<inode>(new inode(_module->load_root(_module->opaque,
			const_cast<char*>(params.c_str())), this));
	}
	// ---------------------------------------------------------------------
	size_t filesystem::max_name_length() const noexcept
	{
		return _module->maximal_name_length(_module->opaque);
	}
	// ---------------------------------------------------------------------
	std::string filesystem::type() const noexcept
	{
		char name[128] = {0};
		_module->get_name(_module->opaque, name, sizeof(name));
		return name;
	}
	// ======================================================================
	stats::stats()
	: vfs_inode_stats()
	{
		num_of_additional_attributes = 0;
		additional_attributes = nullptr;
	}
	// ----------------------------------------------------------------------
	stats::~stats()
	{
		destructor(this);
	}
	// =====================================================================
	directory_iterator::~directory_iterator()
	{
		_ops->destructor(_ops);
	}
	// ----------------------------------------------------------------------
	bool directory_iterator::has_next() const
	{
		return _ops->has_next(_ops->opaque);
	}
	// ----------------------------------------------------------------------
	const inode* directory_iterator::owner() const
	{
		return _owner;
	}
	// ----------------------------------------------------------------------
	std::string directory_iterator::next()
	{
		std::string s;
		s.resize(_owner->owner()->max_name_length());
		size_t n = _ops->next(_ops->opaque, const_cast<char*>(s.c_str()), s.size());
		s.resize(n);
		return s;
	}
	// ----------------------------------------------------------------------
	directory_iterator::directory_iterator(vfs_directory_iterator* ops, const inode* owner)
	:_ops(ops), _owner(owner)
	{
	}
	// ========================================================================
	std::unique_ptr<inode> inode::lookup(const std::string& name) const
	{
		vfs_inode_ops* child = _ops->lookup(_ops->opaque, const_cast<char*>(name.c_str()));
		if (!child)
		{
			return nullptr;
		}
		return std::unique_ptr<inode>(new inode(child, _owner));
	}
	// ----------------------------------------------------------------------
	void inode::stat(stats& st) const
	{
		_ops->stat(_ops->opaque, &st);
	}
	// ----------------------------------------------------------------------
	std::unique_ptr<directory_iterator> inode::get_directory_iterator() const
	{
		vfs_directory_iterator* itr = _ops->get_directory_iterator(_ops->opaque);
		if (!itr)
		{
			return nullptr;
		}
		return std::unique_ptr<directory_iterator>(new directory_iterator(itr, this));
	}
	// ----------------------------------------------------------------------
	bool inode::mkdir(const std::string& name) const
	{
		return _ops->mkdir(_ops->opaque, const_cast<char*>(name.c_str())) == 1;
	}
	// ----------------------------------------------------------------------
	inode::inode(vfs_inode_ops* ops, filesystem* owner)
	: _ops(ops),
	_owner(owner)
	{

	}
	// ----------------------------------------------------------------------
	inode::~inode()
	{
		if (_ops)
		{
			_ops->destructor(_ops);
		}
	}
	// ----------------------------------------------------------------------
	const filesystem* inode::owner() const
	{
		return _owner;
	}
} // ns vfs
