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
};


REGISTER_FVS_MODULE(physfs)





