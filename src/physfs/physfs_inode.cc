//
// Created by igor on 02/01/2020.
//

#include "physfs_inode.hh"

class directory_iterator : public vfs::module::directory_iterator
{
public:
	directory_iterator(stdfs::directory_iterator di)
	: _di (di),
	_itr(begin(_di)),
	_end(end(_di))
	{

	}

	bool has_next () override
	{
		return _itr != _end;
	}

	std::string next() override
	{
		std::string ret = _itr->path().filename().string();
		_itr++;
		return ret;
	}
private:
	stdfs::directory_iterator _di;
	stdfs::directory_iterator _itr;
	stdfs::directory_iterator _end;
};


// ===================================================================================
physfs_inode::physfs_inode(const stdfs::path& pth)
	: vfs::module::inode(stdfs::is_directory(pth) ? VFS_INODE_DIRECTORY : VFS_INODE_REGULAR),
	  _path(pth)
{

}
// -----------------------------------------------------------------------------------
vfs::module::inode* physfs_inode::lookup(const char* name)
{
	const stdfs::path child = _path / name;
	if (!stdfs::exists(child))
	{
		return nullptr;
	}
	return new physfs_inode(child);
}
// -----------------------------------------------------------------------------------
vfs::module::directory_iterator* physfs_inode::get_directory_iterator()
{
	return new directory_iterator(stdfs::directory_iterator(_path));
}
// -----------------------------------------------------------------------------------
uint64_t physfs_inode::size()
{
	if (VFS_INODE_DIRECTORY == type())
	{
		return 0;
	}
	return stdfs::file_size(_path);
}


