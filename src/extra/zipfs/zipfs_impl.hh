//
// Created by igor on 2/4/24.
//

#ifndef VFS_SRC_EXTRA_ZIPFS_ZIPFS_IMPL_HH_
#define VFS_SRC_EXTRA_ZIPFS_ZIPFS_IMPL_HH_

#include "vfs/api/vfs_module.h"
#include <istream>

namespace vfs::extra {
	class zipfs : public vfs::module::filesystem {
	 public:
		zipfs ();
		~zipfs() override;
	 private:
		vfs::module::inode* load_root (const std::string& params) override;
		size_t max_name_length () override;
		int sync () override;
		int sync_inode (vfs::module::inode* inod) override;
		[[nodiscard]] bool is_readonly () const override;
	 private:
		std::istream* m_stream;
	};
}

#endif //VFS_SRC_EXTRA_ZIPFS_ZIPFS_IMPL_HH_
