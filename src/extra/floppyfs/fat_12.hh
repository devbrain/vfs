//
// Created by igor on 3/4/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FAT_12_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FAT_12_HH_

#include <istream>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <bsw/io/binary_reader.hh>

namespace vfs::extra {
	struct file_map {
		file_map (size_t offset, uint16_t size);
		std::size_t offset;
		uint16_t    size;
	};
	class fat12 {
	 public:
		using dir_handler_t = std::function<bool(uint16_t cluster, bool is_dir, uint32_t size, const std::string& name)>;
	 public:
		explicit fat12(std::istream &is);

		void read_dir(uint16_t cluster, const dir_handler_t& handler) const;
		std::vector<file_map> get_file_map(uint16_t cluster, std::size_t size_of_file) const;
		~fat12();
	 private:
		static int is_end_of_file(uint16_t cluster);
		void get_file_map_helper(uint16_t cluster, uint32_t bytes_remaining, std::vector<file_map>& out) const;
	 private:
		std::size_t cluster_offset(uint16_t cluster) const;
		uint16_t get_fat_entry(uint16_t cluster) const;
	 private:
		mutable bsw::io::binary_reader m_reader;
		uint16_t               m_bytes_per_sector{};
		uint8_t                m_sectors_per_cluster{};
		uint16_t               m_root_dir_entries{};
		uint16_t               m_reserved_sectors;
		std::size_t            m_root_start{};
		std::size_t            m_cluster_start{};
		std::size_t            m_total_sectors{};
	};
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FAT_12_HH_
