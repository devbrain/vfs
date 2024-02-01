#ifndef VFS_DETAIL_DENTRY_HH
#define VFS_DETAIL_DENTRY_HH

#include <map>
#include <memory>
#include <tuple>
#include "api/detail/path.hh"
#include "api/detail/wrapped_pointer.hh"

namespace vfs::core {
	class mount_point;

	struct dentry;

	class inode;

	void dentry_init (wrapped_pointer<mount_point> wp);

	void dentry_done ();

	std::tuple<dentry*, std::shared_ptr<inode>, int, path> dentry_resolve (const path& pth);

	// unlink all dentries, and returns true iff this dentry is a mount point
	bool dentry_unlink (dentry* victim);

	bool dentry_has_children (dentry* victim);

	void dentry_mount (wrapped_pointer<mount_point> wp, dentry* node);

	void dentry_set_cwd(const std::string& wd);
	std::string dentry_get_cwd();

}

#endif
