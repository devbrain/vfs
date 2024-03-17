//
// Created by igor on 3/14/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FAT_DRIVER_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FAT_DRIVER_HH_

#include <istream>
#include <ostream>
#include <cstdint>
#include <vector>
#include <optional>

#include "fat/bpb.hh"
#include "fat/fat.hh"
#include "fat/directory.hh"

namespace vfs::extra {
	class driver {
	 public:
		struct file_map {
			file_map (size_t offset, uint16_t size);
			std::size_t offset;
			uint16_t    size;
		};
	 public:
		explicit driver(std::istream& is);

		std::istream& stream();
		[[nodiscard]] const std::istream& stream() const;

		// fat related
		[[nodiscard]] uint32_t fat_get(uint32_t cluster) const;
		void fat_set(uint32_t cluster, uint32_t value);
		void fat_set(uint32_t cluster, fat::cluster_type_t t);
		[[nodiscard]] uint32_t fat_size() const;
		[[nodiscard]] std::optional<fat::cluster_type_t> get_cluster_type(uint32_t value) const;

		[[nodiscard]] std::vector<file_map> get_file_map(uint32_t cluster, std::size_t size_of_file) const;

		[[nodiscard]] std::size_t  get_cluster_offset(uint32_t cluster) const;
		[[nodiscard]] std::size_t  get_bytes_per_cluster() const;

		// directory
		directory get_root_directory();
		directory get_directory (uint32_t cluster);
	 private:
		void get_file_map_helper (uint32_t cluster, uint32_t bytes_remaining, std::vector<file_map>& out) const;
	 private:
		std::istream&    m_istream;
		bpb              m_bpb;
		std::vector<fat> m_fat;
	};

	std::ostream& operator<< (std::ostream& os, const directory::entry& entry);
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FAT_DRIVER_HH_
