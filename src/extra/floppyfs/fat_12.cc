//
// Created by igor on 3/4/24.
//

#include "fat_12.hh"
#include <cstring>
#include <iostream>
#include <bsw/exception.hh>
#include <bsw/byte_order.hh>

#if defined(BSW_IS_LITTLE_ENDIAN)
#define    getushort(x)    *((u_int16_t *)(x))
#define    getulong(x)    *((u_int32_t *)(x))
#define    putushort(p, v)    (*((u_int16_t *)(p)) = (v))
#define    putulong(p, v)    (*((u_int32_t *)(p)) = (v))
#else
#define getushort(x)    (((u_int8_t *)(x))[0] + (((u_int8_t *)(x))[1] << 8))
#define getulong(x)    (((u_int8_t *)(x))[0] + (((u_int8_t *)(x))[1] << 8) \
             + (((u_int8_t *)(x))[2] << 16)    \
             + (((u_int8_t *)(x))[3] << 24))
#define putushort(p, v)    (((u_int8_t *)(p))[0] = (v),    \
             ((u_int8_t *)(p))[1] = (v) >> 8)
#define putulong(p, v)    (((u_int8_t *)(p))[0] = (v),    \
             ((u_int8_t *)(p))[1] = (v) >> 8, \
             ((u_int8_t *)(p))[2] = (v) >> 16,\
             ((u_int8_t *)(p))[3] = (v) >> 24)
#endif

namespace vfs::extra {

	namespace {
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

		bsw::io::binary_reader& operator>> (bsw::io::binary_reader& is, bpb& out) {
			is.read_raw (out.jmp, sizeof (out.jmp));
			is.read_raw (out.oem, sizeof (out.oem));
			is >> out.bytes_per_sector >> out.sectors_per_cluster
			   >> out.reserved_sectors >> out.number_of_fats
			   >> out.root_dir_entries >> out.total_sectors
			   >> out.media_descriptor >> out.sectors_per_fat
			   >> out.heads >> out.number_of_hidden_sectors
			   >> out.large_sector_count;
			return is;
		}

		// ===============================================================
		struct dos_dirent {
			uint8_t deName[8];    /* filename, blank filled */
#define    SLOT_EMPTY    0x00        /* slot has never been used */
#define    SLOT_E5        0x05        /* the real value is 0xe5 */
#define    SLOT_DELETED    0xe5        /* file in this slot deleted */
			uint8_t deExtension[3];    /* extension, blank filled */
			uint8_t deAttributes;    /* file attributes */
#define    ATTR_NORMAL    0x00        /* normal file */
#define    ATTR_READONLY    0x01        /* file is readonly */
#define    ATTR_HIDDEN    0x02        /* file is hidden */
#define    ATTR_SYSTEM    0x04        /* file is a system file */
#define    ATTR_VOLUME    0x08        /* entry is a volume label */
#define    ATTR_DIRECTORY    0x10        /* entry is a directory name */
#define    ATTR_ARCHIVE    0x20        /* file is new or modified */
			uint8_t deLowerCase;    /* NT VFAT lower case flags */
#define    LCASE_BASE    0x08        /* filename base in lower case */
#define    LCASE_EXT    0x10        /* filename extension in lower case */
			uint8_t deCHundredth;    /* hundredth of seconds in CTime */
			uint8_t deCTime[2];    /* create time */
			uint8_t deCDate[2];    /* create date */
			uint8_t deADate[2];    /* access date */
			uint8_t deHighClust[2];    /* high bytes of cluster number */
			uint8_t deMTime[2];    /* last update time */
			uint8_t deMDate[2];    /* last update date */
			uint8_t deStartCluster[2]; /* starting cluster of file */
			uint8_t deFileSize[4];    /* size of file in bytes */
		};

#define    MSDOSFSROOT    0        /* cluster 0 means the root dir */
#define    CLUST_FREE    0        /* cluster 0 also means a free cluster */
#define    MSDOSFSFREE    CLUST_FREE
#define    CLUST_FIRST    2        /* first legal cluster number */
#define    CLUST_RSRVD    0xfffffff6    /* reserved cluster range */
#define    CLUST_BAD    0xfffffff7    /* a cluster with a defect */
#define    CLUST_EOFS    0xfffffff8    /* start of eof cluster range */
#define    CLUST_EOFE    0xffffffff    /* end of eof cluster range */

#define    FAT12_MASK    0x00000fff    /* mask for 12 bit cluster numbers */
#define    FAT16_MASK    0x0000ffff    /* mask for 16 bit cluster numbers */
#define    FAT32_MASK    0x0fffffff    /* mask for FAT32 cluster numbers */
	}

	fat12::fat12 (std::istream& is)
		: m_reader (is, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER) {
		static_assert (sizeof (dos_dirent) == 32);
		bpb boot_sec{};
		m_reader >> boot_sec;

		m_bytes_per_sector = boot_sec.bytes_per_sector;
		m_sectors_per_cluster = boot_sec.sectors_per_cluster;
		m_root_dir_entries = boot_sec.root_dir_entries;
		m_root_start = boot_sec.reserved_sectors + boot_sec.number_of_fats * boot_sec.sectors_per_fat;
		m_reserved_sectors = boot_sec.reserved_sectors;
		m_cluster_start = m_root_start + (boot_sec.root_dir_entries * 32) / boot_sec.bytes_per_sector;
		if ((boot_sec.root_dir_entries * 32) % boot_sec.bytes_per_sector) {
			m_cluster_start++;
		}
		m_total_sectors = boot_sec.total_sectors;
	}

	void fat12::read_dir (uint16_t cluster, const dir_handler_t& handler) const {
		dos_dirent dirent{};
		auto offs = cluster_offset (cluster);
		m_reader.stream ().seekg (offs, std::ios::beg);
		m_reader.read_raw ((char*)&dirent, sizeof (dirent));
		bool quit = false;
		while (!quit) {
			for (std::size_t d = 0; d < m_bytes_per_sector * m_sectors_per_cluster; d += sizeof (dos_dirent)) {
				char name[9];
				char extension[4];
				uint32_t size;
				uint16_t file_cluster;
				name[8] = ' ';
				extension[3] = ' ';
				memcpy (name, dirent.deName, 8);
				memcpy (extension, dirent.deExtension, 3);
				if ((uint8_t)name[0] == SLOT_EMPTY) {
					quit = true;
					break;
				}
				if ((uint8_t)name[0] == SLOT_DELETED) {
					continue;
				}
				for (int i = 8; i > 0; i--) {
					if (name[i] == ' ') {
						name[i] = '\0';
					} else {
						break;
					}
				}

				for (int i = 3; i >= 0; i--) {
					if (extension[i] == ' ') {
						extension[i] = '\0';
					} else {
						break;
					}
				}
				if (strcmp (name, ".") == 0) {
					m_reader.read_raw ((char*)&dirent, sizeof (dirent));
					continue;
				}
				if (strcmp (name, "..") == 0) {
					m_reader.read_raw ((char*)&dirent, sizeof (dirent));
					continue;
				}

				std::string fname = strlen (extension) > 0 ? std::string (name) + "." + extension : name;
				if ((dirent.deAttributes & ATTR_VOLUME) != 0) {

				} else if ((dirent.deAttributes & ATTR_DIRECTORY) != 0) {
					file_cluster = getushort(dirent.deStartCluster);
					if (!handler (file_cluster, true, 0, fname)) {
						return;
					}
				} else {
					size = getulong(dirent.deFileSize);
					file_cluster = getushort(dirent.deStartCluster);
					if (!handler (file_cluster, false, size, fname)) {
						return;
					}
				}
				m_reader.read_raw ((char*)&dirent, sizeof (dirent));
			}
			if (!quit) {
				if (cluster == 0) {
					m_reader.read_raw ((char*)&dirent, sizeof (dirent));
				} else {
					cluster = get_fat_entry (cluster);
					offs = cluster_offset (cluster);
					m_reader.stream ().seekg (offs, std::ios::beg);
					m_reader.read_raw ((char*)&dirent, sizeof (dirent));
				}
			}
		}
	}

	std::size_t fat12::cluster_offset (uint16_t cluster) const {
		std::size_t p = m_bytes_per_sector * m_root_start;
		if (cluster != MSDOSFSROOT) {
			p += m_root_dir_entries * sizeof (dos_dirent);
			p += m_bytes_per_sector * m_sectors_per_cluster * (cluster - CLUST_FIRST);
		}
		return p;
	}

	uint16_t fat12::get_fat_entry (uint16_t cluster) const {
		uint32_t offset;
		uint16_t value;
		uint8_t b1, b2;

		offset = m_reserved_sectors* m_bytes_per_sector + (3 * (cluster/2));
		auto current = m_reader.stream().tellg();
		switch(cluster % 2) {
			case 0:
				m_reader.stream().seekg (offset, std::ios::beg);
				m_reader >> b1 >> b2;
				value = ((0x0f & b2) << 8) | b1;
				break;
			case 1:
				m_reader.stream().seekg (offset + 1, std::ios::beg);
				value = b2 << 4 | ((0xf0 & b1) >> 4);
				break;
		}
		m_reader.stream().seekg (current);
		return value;
	}

	int fat12::is_end_of_file (uint16_t cluster) {
		return (cluster >= (FAT12_MASK & CLUST_EOFS) && cluster <= (FAT12_MASK & CLUST_EOFE));
	}

	std::vector<file_map> fat12::get_file_map (uint16_t cluster, std::size_t size_of_file) const {
		std::vector<file_map> out;
		get_file_map_helper (cluster, size_of_file, out);
		return out;
	}

	void fat12::get_file_map_helper (uint16_t cluster, uint32_t bytes_remaining, std::vector<file_map>& out) const {
		uint32_t clust_size = m_sectors_per_cluster * m_bytes_per_sector;
		auto total_clusters = m_total_sectors / m_sectors_per_cluster;
		if (cluster == 0) {
			RAISE_EX("Bad file termination");
		} else if (is_end_of_file(cluster)) {
			return;
		} else if (cluster > total_clusters) {
			RAISE_EX("Too many clusters");
		}

		/* map the cluster number to the data location */
		auto offset = cluster_offset(cluster);

		if (bytes_remaining <= clust_size) {
			out.emplace_back (offset, bytes_remaining);
		} else {
			/* more clusters after this one */
			out.emplace_back (offset, clust_size);
			/* recurse, continuing to copy */
			get_file_map_helper(get_fat_entry(cluster), bytes_remaining - clust_size, out);
		}
	}

	fat12::~fat12 () = default;

	file_map::file_map (size_t offset, uint16_t size)
		: offset (offset), size (size) {}
}