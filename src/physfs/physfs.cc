//
// Created by igor on 25/12/2019.
//

#include "vfs/api/vfs_module.h"
#include "physfs_inode.hh"


class physfs : public vfs::module::filesystem
{
public:
	explicit physfs()
		: vfs::module::filesystem("physfs")
	{

	}

private:
	vfs::module::inode* load_root(const std::string& params) override
	{
		return new physfs_inode(stdfs::path(params));
	}

	size_t max_name_length() override
	{
		return 64;
	}
	int sync() override
	{
		return 1;
	}

	int sync_inode(vfs::module::inode* /*inod*/)
	{
		return 1;
	}
};


REGISTER_VFS_MODULE(physfs)





