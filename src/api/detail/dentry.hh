#ifndef VFS_DETAIL_DENTRY_HH
#define VFS_DETAIL_DENTRY_HH


#include <map>
#include <memory>
#include <tuple>
#include <vfs/api/path.hh>
#include "api/detail/wrapped_pointer.hh"

namespace vfs::core
{
	class mount_point;
	struct dentry;
	class inode;

	void dentry_init (wrapped_pointer<mount_point> wp);
	void dentry_done ();

	std::tuple<dentry*, inode*, int> dentry_resolve(const path& pth, int from, int to);
}

#endif
