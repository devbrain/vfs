//
// Created by igor on 3/14/24.
//

#include <algorithm>
#include <utility>
#include <cstring>
#include <variant>
#include "driver.hh"
#include "directory.hh"
#include <bsw/byte_order.hh>
#include <bsw/override.hh>
#include <bsw/strings/wchar.hh>

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
	struct dos_dirent {
		uint8_t deName[11];    /* filename, blank filled */
#define    SLOT_EMPTY    0x00        /* slot has never been used */
#define    SLOT_E5        0x05        /* the real value is 0xe5 */
#define    SLOT_DELETED    0xe5        /* file in this slot deleted */
		uint8_t deAttributes;    /* file attributes */
#define    ATTR_NORMAL    0x00        /* normal file */
#define    ATTR_READONLY    0x01        /* file is readonly */
#define    ATTR_HIDDEN    0x02        /* file is hidden */
#define    ATTR_SYSTEM    0x04        /* file is a system file */
#define    ATTR_VOLUME    0x08        /* entry is a volume label */
#define    ATTR_DIRECTORY    0x10        /* entry is a directory name */

#define    ATTR_ARCHIVE    0x20        /* file is new or modified */
#define    ATTR_LFN     0xf // (ATTR_READONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME | ATTR_DIRECTORY | ATTR_ARCHIVE)
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

	struct lfn_data {
		uint8_t LDIR_Ord;
		uint8_t LDIR_Name1[10];
		uint8_t LDIR_Attr;
		uint8_t LDIR_Type;
		uint8_t LDIR_Chksum;
		uint8_t LDIR_Name2[12];
		uint16_t LDIR_FstClusLO;
		uint8_t LDIR_Name3[4];
	};

	static constexpr std::size_t DIR_ENTRY_SIZE = 32;

	struct slot_deleted {};
	struct slot_empty {};

	struct slot_entry {
		uint32_t cluster;
		uint32_t size;
		uint8_t  name[11];
		bool     is_dir;
	};
	static std::variant<slot_deleted, slot_empty, slot_entry, lfn_data>
	read_dir_entry (std::istream& is) {
		static_assert (sizeof (dos_dirent) == DIR_ENTRY_SIZE);
		static_assert (sizeof (lfn_data) == DIR_ENTRY_SIZE);

		char buff[DIR_ENTRY_SIZE];
		dos_dirent de {};
		is.read (buff, DIR_ENTRY_SIZE);
		std::memcpy (&de, buff, DIR_ENTRY_SIZE);
		if (de.deName[0] == SLOT_DELETED) {
			return slot_deleted{};
		}
		if (de.deName[0] == SLOT_EMPTY) {
			return slot_empty{};
		}
		if ((de.deAttributes & ATTR_LFN) == ATTR_LFN) {
			lfn_data d{};
			memcpy (&d, buff, sizeof(d));
			return d;
		}
		if ((de.deAttributes & ATTR_DIRECTORY) != 0) {
			slot_entry sd {};
			sd.is_dir = true;
			sd.cluster = getushort(de.deStartCluster);
			std::memcpy(sd.name, de.deName, sizeof(sd.name));
			return sd;
		} else {
			slot_entry sf {};
			sf.is_dir = false;
			sf.size = getulong(de.deFileSize);
			sf.cluster = getushort(de.deStartCluster);
			std::memcpy(sf.name, de.deName, sizeof(sf.name));
			return sf;
		}
	}
	// ===================================================================================
	directory::directory (uint32_t cluster, std::size_t bytes_per_cluster, driver& dos_fs)
		: m_driver (dos_fs),
		  m_cluster (cluster),
		  m_dir_entries (bytes_per_cluster / DIR_ENTRY_SIZE) {
	}

	directory::directory (driver& dos_fs, std::size_t root_dir_entries)
		: m_driver (dos_fs),
		  m_cluster (0),
		  m_dir_entries (root_dir_entries) {
	}

	directory::iterator directory::get_iterator () {
		return directory::iterator(*this);
	}

	directory::entry::entry (std::string name, uint32_t size, bool is_dir, uint32_t cluster)
		: name (std::move(name)), size (size), is_dir (is_dir), cluster (cluster) {}

	std::ostream& operator<< (std::ostream& os, const directory::entry& entry) {
		os << "name: " << entry.name << " size: " << entry.size << " is_dir: " << entry.is_dir << " cluster: "
		   << entry.cluster;
		return os;
	}

	// ==================================================================================
	directory::iterator::iterator (directory& owner)
	: m_owner (owner),
	m_current_cluster(owner.m_cluster),
	m_pos(0) {

	}

	struct parsed_lfn {
		uint8_t seq;
		bool    is_last;
		uint16_t name1[sizeof (lfn_data::LDIR_Name1) / sizeof (uint16_t)];
		uint16_t name2[sizeof (lfn_data::LDIR_Name2) / sizeof (uint16_t)];
		uint16_t name3[sizeof (lfn_data::LDIR_Name3) / sizeof (uint16_t)];
	};

	directory::entry generate_entry(const slot_entry& e, std::vector<parsed_lfn>& lfn) {
		if (lfn.empty()) {
			std::string name;
			std::string ext;
			for (int i=0; i<8; i++) {
				if (e.name[i] != ' ') {
					name += e.name[i];
				}
			}
			for (int i=8; i<11; i++) {
				if (e.name[i] != ' ') {
					ext += e.name[i];
				}
			}
			std::string oname = ext.empty() ? name : name + "." + ext;
			return {oname, e.size, e.is_dir, e.cluster};
		}
		if (lfn.size() > 1) {
			std::sort (lfn.begin (), lfn.end (), [] (const auto& a, const auto& b) {
			  return a.seq < b.seq;
			});
		}
		std::wstring ws;
		for (const auto& lfn_entry: lfn) {
			bool is_end = false;
			for (auto rune : lfn_entry.name1) {
				if (rune == 0 || rune == 0xFFFF) {
					is_end = true;
					break;
				} else {
					wchar_t w = bsw::byte_order::from_little_endian (rune);
					ws += w;
				}
			}
			if (!is_end) {
				for (auto rune : lfn_entry.name2) {
					if (rune == 0 || rune == 0xFFFF) {
						is_end = true;
						break;
					} else {
						wchar_t w = bsw::byte_order::from_little_endian (rune);
						ws += w;
					}
				}
			}
			if (!is_end) {
				for (auto rune : lfn_entry.name3) {
					if (rune == 0 || rune == 0xFFFF) {
						is_end = true;
						break;
					} else {
						wchar_t w = bsw::byte_order::from_little_endian (rune);
						ws += w;
					}
				}
			}
		}
		return {bsw::wstring_to_utf8 (ws), e.size, e.is_dir, e.cluster};
	}

	bool directory::iterator::reposition() {
		if (m_current_cluster == 0) {
			if (m_pos >= m_owner.m_dir_entries) {
				return false;
			}
		} else {
			if (m_pos > 0) {
				if (m_pos >= m_owner.m_dir_entries) {
					m_pos = 0;
					m_current_cluster = m_owner.m_driver.fat_get (m_current_cluster);
				}
			}
		}
		auto offs = m_owner.m_driver.get_cluster_offset (m_current_cluster) + DIR_ENTRY_SIZE*m_pos;
		auto& ifs = m_owner.m_driver.stream();
		ifs.seekg (offs);
		return true;
	}

	std::optional<directory::entry> directory::iterator::read () {
		auto& ifs = m_owner.m_driver.stream();

		bool read_more = true;
		bool end_of_scan = false;
		std::vector<parsed_lfn> lfn;

		while (read_more) {
			if (!reposition()) {
				return std::nullopt;
			}
			auto rc = read_dir_entry (ifs);
			m_pos++;

			// slot_deleted, slot_empty, slot_dir, slot_file
			auto entry = std::visit (bsw::overload (
				[&read_more] (const slot_deleted&) -> std::optional<slot_entry> {
					read_more = true;
				  	return std::nullopt;
				},
				[&read_more, &end_of_scan] (const slot_empty&)-> std::optional<slot_entry> {
				  read_more = false;
				  end_of_scan = true;
				  return std::nullopt;
				},
				[&read_more] (const slot_entry& e)-> std::optional<slot_entry> {
					if (e.name[0] == '.') {
						if (e.name[1] == ' ' || (e.name[1] == '.' && e.name[2] == ' ')) {
							read_more = true;
							return std::nullopt;
						}
					}
					return e;
				},
				[&lfn](const lfn_data& e) -> std::optional<slot_entry> {
					parsed_lfn p {};
					p.is_last = (e.LDIR_Ord | 0x40) == e.LDIR_Ord;
					p.seq = p.is_last ? e.LDIR_Ord - 0x40 : e.LDIR_Ord;
				  std::memcpy (p.name1, e.LDIR_Name1, sizeof (p.name1));
				  std::memcpy (p.name2, e.LDIR_Name2, sizeof (p.name2));
				  std::memcpy (p.name3, e.LDIR_Name3, sizeof (p.name3));
				  lfn.push_back (p);
				  return std::nullopt;
				}
			), rc);
			if (entry.has_value()) {
				return generate_entry (*entry, lfn);
			}
		}
		if (end_of_scan) {
			return std::nullopt;
		}
	}
}