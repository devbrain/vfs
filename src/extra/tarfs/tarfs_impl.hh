//
// Created by igor on 3/18/24.
//

#ifndef VFS_SRC_EXTRA_TARFS_TARFS_IMPL_HH_
#define VFS_SRC_EXTRA_TARFS_TARFS_IMPL_HH_

#include "vfs/api/vfs_module.h"
#include "tarfile.hh"

namespace vfs::extra {
	class tarfs_impl  : public vfs::module::filesystem {
	 public:
		tarfs_impl ();
		~tarfs_impl () override;
	 private:
		vfs::module::inode* load_root (const std::string& params) override;
		size_t max_name_length () override;
		int sync () override;
		int sync_inode (vfs::module::inode* inod) override;
		[[nodiscard]] bool is_readonly () const override;
	 private:
		std::unique_ptr<std::istream> m_istream;
		std::unique_ptr<tarfile> m_tar;
	};
}

#endif //VFS_SRC_EXTRA_TARFS_TARFS_IMPL_HH_
