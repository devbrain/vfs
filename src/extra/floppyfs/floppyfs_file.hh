//
// Created by igor on 3/10/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_FILE_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_FILE_HH_

#include "fat_12.hh"
#include <vfs/api/vfs_module.h>

namespace vfs::extra {
	class floppyfs_file : public vfs::module::file {
	 public:
		floppyfs_file(fat12* fat, uint16_t start_cluster);
		~floppyfs_file() override;
	 private:
		ssize_t read (void* buff, size_t len) override;
		ssize_t write (void* buff, size_t len) override;
		bool seek ([[maybe_unused]] uint64_t pos, [[maybe_unused]] whence_type whence) override;
		bool truncate () override;
		[[nodiscard]] uint64_t tell () const override;
	 private:
		fat12*   m_fat;
		uint16_t m_start_cluster;

		uint64_t m_pointer;
	};
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_FILE_HH_
