//
// Created by igor on 3/4/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_BPB_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_BPB_HH_

#include <cstdint>
#include <bsw/io/binary_reader.hh>

namespace vfs::extra {
	struct bpb {
		uint8_t jmp[3];
		char oem[8];
		uint16_t bytes_per_sector;
		uint8_t sectors_per_cluster;
		uint16_t reserved_sectors;
		uint8_t number_of_fats;
		uint16_t root_dir_entries;
		uint16_t total_sectors;
		uint8_t media_descriptor;
		uint16_t sectors_per_fat;
		uint16_t heads;
		uint32_t number_of_hidden_sectors;
		uint32_t large_sector_count;
	};

	bsw::io::binary_reader& operator >> (bsw::io::binary_reader& is, bpb& out);
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_BPB_HH_
