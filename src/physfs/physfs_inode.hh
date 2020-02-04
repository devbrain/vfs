#ifndef PHYSFS_INODE_HH
#define PHYSFS_INODE_HH

#include "vfs/api/vfs_module.h"
#include "vfs/api/stdfilesystem.hh"


class physfs_inode : public vfs::module::inode
{
public:
	explicit physfs_inode(const stdfs::path& pth);

	inode* lookup(const char* name) override ;
	uint64_t size() override ;
	vfs::module::directory_iterator* get_directory_iterator() override;
	bool mkdir (const char* name) override ;
	int unlink() override;

	vfs::module::file* open_file(open_mode_type mode_type) override ;
private:
	const stdfs::path _path;
};


#endif
