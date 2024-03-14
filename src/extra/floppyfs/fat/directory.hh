//
// Created by igor on 3/14/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FAT_DIRECTORY_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FAT_DIRECTORY_HH_

#include <istream>
#include <cstdint>
#include <string>
#include <optional>

namespace vfs::extra {
	class driver;

	class directory {
	 public:
		struct entry {
			entry (std::string  name, uint32_t size, bool is_dir, uint32_t cluster);
			std::string name;
			uint32_t    size;
			bool        is_dir;
			uint32_t    cluster;
		};

		class iterator {
			friend class directory;
		 public:
			iterator(const iterator&) = delete;
			iterator& operator = (const iterator&) = delete;

			iterator(iterator&&) = default;
			iterator& operator = (iterator&&) = delete;

			std::optional<entry> read();
		 private:
			explicit iterator(directory& owner);
		 private:
			directory& m_owner;
			uint32_t   m_current_cluster;
			uint32_t   m_pos;
		};
		friend class iterator;
	 public:
		directory(uint32_t cluster, driver& dos_fs);
		directory(driver& dos_fs, std::size_t root_dir_entries);

		iterator get_iterator();
	 private:
		driver&       m_driver;
		uint32_t      m_cluster;
		std::size_t   m_dir_entries;
	};
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FAT_DIRECTORY_HH_
