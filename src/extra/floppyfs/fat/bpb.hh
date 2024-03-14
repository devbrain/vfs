//
// Created by igor on 3/11/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FAT_BPB_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FAT_BPB_HH_

#include <cstdint>
#include <istream>
#include <vector>

namespace vfs::extra {

	struct disk_entry {
		disk_entry (uint32_t size, uint32_t offset);
		disk_entry(const disk_entry&) = default;
		disk_entry& operator = (const disk_entry&) = default;

		uint32_t size;
		uint32_t offset;
	};

	struct bpb {

		enum type_t {
			FAT12,
			FAT16,
			FAT32
		};


		bpb (type_t type,
			 uint32_t count_of_clusters,
			 std::vector<disk_entry> fat,
			 disk_entry root_directory,
			 uint32_t bytes_per_sector,
			 uint32_t sectors_per_cluster);

		const type_t   type;
		const uint32_t count_of_clusters;
		const std::vector<disk_entry> fat;
		const disk_entry root_directory;
		const uint32_t bytes_per_sector;
		const uint32_t sectors_per_cluster;
	};

	bpb read_bpb(std::istream& is);
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FAT_BPB_HH_
