//
// Created by igor on 25/12/2019.
//

#include "vfs/api/vfs_module.h"
#include "physfs_inode.hh"
#include "physfs.hh"

namespace vfs::detail
{
    physfs::physfs()
            : vfs::module::filesystem("physfs")
    {

    }

    vfs::module::inode* physfs::load_root(const std::string& params)
    {
        return new physfs_inode(stdfs::path(params));
    }

    size_t physfs::max_name_length()
    {
        return 64;
    }
    int physfs::sync()
    {
        return 1;
    }

    int physfs::sync_inode(vfs::module::inode* /*inod*/)
    {
        return 1;
    }

    bool physfs::is_readonly() const
    {
        return false;
    }
}







