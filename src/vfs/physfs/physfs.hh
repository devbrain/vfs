#ifndef VFS_PHYSFS_HH
#define VFS_PHYSFS_HH

#include "vfs/api/vfs_module.h"

namespace vfs::detail {
	class physfs : public vfs::module::filesystem {
	 public:
		physfs ();
	 private:
		vfs::module::inode* load_root (const std::string& params) override;
		size_t max_name_length () override;
		int sync () override;
		int sync_inode (vfs::module::inode* inod) override;
		[[nodiscard]] bool is_readonly () const override;
	};
}
#endif
