//
// Created by igor on 3/10/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_FILE_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_FILE_HH_

#include "fat/driver.hh"
#include <vfs/api/vfs_module.h>

namespace vfs::extra {
	class floppyfs_file : public vfs::module::file {
	 public:
		floppyfs_file(driver* fat, uint32_t start_cluster, std::size_t file_size);
		~floppyfs_file() override;
	 private:
		ssize_t read (void* buff, size_t len) override;
		ssize_t write (void* buff, size_t len) override;
		bool seek ([[maybe_unused]] uint64_t pos, [[maybe_unused]] whence_type whence) override;
		bool truncate () override;
		[[nodiscard]] uint64_t tell () const override;
	 private:
		ssize_t read_cluster (void* buff, size_t len);
	 private:
		driver*  m_fat;
		uint32_t m_start_cluster;
		uint32_t m_current_cluster;
		std::size_t m_file_size;
		std::size_t m_bytes_per_cluster;
		uint64_t m_pointer;
		std::vector<driver::file_map> m_file_map;
	};
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FLOPPYFS_FILE_HH_
