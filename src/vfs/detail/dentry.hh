#ifndef VFS_DETAIL_DENTRY_HH
#define VFS_DETAIL_DENTRY_HH

#include <map>
#include <memory>
#include <tuple>
#include "detail/path.hh"
#include "detail/wrapped_pointer.hh"

namespace vfs::core {

	class mount_point;
	struct dentry;
	class inode;

	class dentry_tree {
	 public:
		explicit dentry_tree(wrapped_pointer<mount_point> wp);
		~dentry_tree();

		[[nodiscard]] std::tuple<dentry*, std::shared_ptr<inode>, int, path> resolve (const path& pth) const;

		// unlink all dentries, and returns true iff this dentry is a mount point
		static bool unlink (dentry* victim);

		static bool has_children (dentry* q);

		static void mount (wrapped_pointer<mount_point> wp, dentry* node);

		void cwd(const std::string& wd);
		[[nodiscard]] std::string cwd() const;
	 private:
		[[nodiscard]] std::tuple<dentry*, std::shared_ptr<inode>, int, path> _resolve (const path& pth, int from, int to) const;
	 private:
		std::unique_ptr<dentry> m_root;
		path m_current_wd;
	};

}

#endif
