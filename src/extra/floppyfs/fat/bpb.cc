//
// Created by igor on 3/11/24.
//

#include <bsw/io/binary_reader.hh>
#include <bsw/logger/logger.hh>
#include <bsw/exception.hh>
#include <utility>
#include "bpb.hh"

namespace vfs::extra {
#pragma pack(push, 1)
	struct bpb_common {
		uint8_t BS_jmpBoot[3];
		char BS_OEMName[8];
		uint16_t BPB_BytsPerSec;
		uint8_t BPB_SecPerClus;
		uint16_t BPB_RsvdSecCnt;
		uint8_t BPB_NumFATs;
		uint16_t BPB_RootEntCnt;
		uint16_t BPB_TotSec16;
		uint8_t BPB_Media;
		uint16_t BPB_FATSz16;
		uint16_t BPB_SecPerTrk;
		uint16_t BPB_NumHeads;
		uint32_t BPB_HiddSec;
		uint32_t BPB_TotSec32;
		uint32_t BPB_FATSz32; // hack for fat-32
	};
#pragma pack(pop)



	std::istream& operator>> (std::istream& is, bpb_common& out) {
		static_assert (sizeof(bpb_common) == 40);
		bsw::io::binary_reader rdr (is, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);
		rdr.read_raw (out.BS_jmpBoot, sizeof (out.BS_jmpBoot));
		rdr.read_raw (out.BS_OEMName, sizeof (out.BS_OEMName));
		rdr >> out.BPB_BytsPerSec
			>> out.BPB_SecPerClus
			>> out.BPB_RsvdSecCnt
			>> out.BPB_NumFATs
			>> out.BPB_RootEntCnt
			>> out.BPB_TotSec16
			>> out.BPB_Media
			>> out.BPB_FATSz16
			>> out.BPB_SecPerTrk
			>> out.BPB_NumHeads
			>> out.BPB_HiddSec
			>> out.BPB_TotSec32
			>> out.BPB_FATSz32;
		return is;
	}



	bpb_common read_bpb_common (std::istream& is) {
		bpb_common bpb_dos33{};
		is >> bpb_dos33;
		if (bpb_dos33.BS_jmpBoot[0] == 0xEB) {
			if (bpb_dos33.BS_jmpBoot[2] != 0x90) {
				EVLOG_TRACE(EVLOG_INFO, "Suspicious JMP instruction in BPB");
			}
		} else if (bpb_dos33.BS_jmpBoot[0] != 0xE9) {
			EVLOG_TRACE(EVLOG_INFO, "Suspicious JMP instruction in BPB");
		}
		switch (bpb_dos33.BPB_BytsPerSec) {
			case 512:
			case 1024:
			case 2048:
			case 4096:
				break;
			default: RAISE_EX("Bad number of bytes per sector,", bpb_dos33.BPB_BytsPerSec);
		}
		switch (bpb_dos33.BPB_SecPerClus) {
			case 1:
			case 2:
			case 4:
			case 8:
			case 16:
			case 32:
			case 64:
			case 128:
				break;
			default:
				RAISE_EX("Bad number of sectors per cluster,", bpb_dos33.BPB_SecPerClus);
		}
		if (bpb_dos33.BPB_RootEntCnt*32 % bpb_dos33.BPB_BytsPerSec != 0) {
			RAISE_EX("Bad number of root entries,", bpb_dos33.BPB_RootEntCnt);
		} else {
			auto r = bpb_dos33.BPB_RootEntCnt*32 / bpb_dos33.BPB_BytsPerSec;
			if (r % 2 != 0) {
				RAISE_EX("Bad number of root entries,", bpb_dos33.BPB_RootEntCnt);
			}
		}
		if ((bpb_dos33.BPB_TotSec16 == 0) && (bpb_dos33.BPB_TotSec32 == 0)) {
			RAISE_EX("Bad number of total sectors");
		}
		switch (bpb_dos33.BPB_Media) {
			case 0xF0:
			case 0xF8:
			case 0xF9:
			case 0xFA:
			case 0xFB:
			case 0xFC:
			case 0xFD:
			case 0xFE:
			case 0xFF:
				break;
			default:
				RAISE_EX("Bad value of media type,", bpb_dos33.BPB_Media);
		}

		return bpb_dos33;
	}

	bpb read_bpb(std::istream& is) {
		auto bpb_dos33 = read_bpb_common(is);
		uint32_t RootDirSectors = ((bpb_dos33.BPB_RootEntCnt * 32) + (bpb_dos33.BPB_BytsPerSec - 1)) / bpb_dos33.BPB_BytsPerSec;

		uint32_t FATSz  = (bpb_dos33.BPB_FATSz16 != 0) ? bpb_dos33.BPB_FATSz16 : bpb_dos33.BPB_FATSz32;
		uint32_t TotSec = (bpb_dos33.BPB_FATSz16 != 0) ? bpb_dos33.BPB_TotSec16 : bpb_dos33.BPB_TotSec32;

		auto DataSec = TotSec - (bpb_dos33.BPB_RsvdSecCnt + (bpb_dos33.BPB_NumFATs * FATSz) + RootDirSectors);
		auto CountofClusters = DataSec / bpb_dos33.BPB_SecPerClus;

		bpb::type_t type = bpb::FAT12;
		if(CountofClusters < 4085) {
			type = bpb::FAT12;
		} else if(CountofClusters < 65525) {
			type = bpb::FAT16;
		} else {
			type = bpb::FAT32;
		}

		uint32_t fat_offset = bpb_dos33.BPB_RsvdSecCnt * bpb_dos33.BPB_BytsPerSec;
		auto fat_size = FATSz * bpb_dos33.BPB_BytsPerSec;

		std::vector<disk_entry> fats;
		fats.reserve (bpb_dos33.BPB_NumFATs);
		fats.emplace_back (fat_size, fat_offset);
		for (std::size_t i=1; i<bpb_dos33.BPB_NumFATs; i++) {
			fat_offset = fats.back().offset + fat_size;
			fats.emplace_back (fat_size, fat_offset);
		}

		auto root_dir_offset = (bpb_dos33.BPB_RsvdSecCnt + (bpb_dos33.BPB_NumFATs * FATSz)) * bpb_dos33.BPB_BytsPerSec;
		auto root_dir_size = RootDirSectors * bpb_dos33.BPB_BytsPerSec;

		return {type,
				CountofClusters,
				fats,
				{root_dir_size, root_dir_offset},
				bpb_dos33.BPB_BytsPerSec,
				bpb_dos33.BPB_SecPerClus,
				bpb_dos33.BPB_RootEntCnt
		};
	}



	disk_entry::disk_entry (uint32_t size, uint32_t offset)
		: size (size), offset (offset) {}


	bpb::bpb (bpb::type_t type,
			  uint32_t count_of_clusters,
			  std::vector<disk_entry> fat,
			  disk_entry root_directory,
			  uint32_t bytes_per_sector,
			  uint32_t sectors_per_cluster,
			  uint32_t root_dir_entries)
		:
		type (type),
		count_of_clusters (count_of_clusters),
		fat (std::move(fat)),
		root_directory (root_directory),
		bytes_per_sector (bytes_per_sector),
		sectors_per_cluster(sectors_per_cluster),
		root_dir_entries (root_dir_entries) {

	}
}