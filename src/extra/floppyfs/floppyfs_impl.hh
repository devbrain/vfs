//
// Created by igor on 3/5/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_IMPL_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_IMPL_HH_

#include <memory>
#include <iostream>
#include <vfs/api/vfs_module.h>
#include "fat/driver.hh"
namespace vfs::extra {
	class floppyfs : public vfs::module::filesystem {
	 public:
		floppyfs ();
		~floppyfs () override;
	 private:
		vfs::module::inode* load_root (const std::string& params) override;
		size_t max_name_length () override;
		int sync () override;
		int sync_inode (vfs::module::inode* inod) override;
		[[nodiscard]] bool is_readonly () const override;
	 private:
		std::unique_ptr<std::iostream> m_istream;
		std::unique_ptr<driver> m_fat;
	};

}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_IMPL_HH_
