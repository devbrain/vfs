//
// Created by igor on 3/14/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FAT_DRIVER_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FAT_DRIVER_HH_

#include <istream>
#include <cstdint>
#include <vector>
#include <optional>

#include "fat/bpb.hh"
#include "fat/fat.hh"

namespace vfs::extra {
	class driver {
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

		[[nodiscard]] std::size_t  get_cluster_offset(uint32_t cluster) const;

	 private:
		std::istream&    m_istream;
		bpb              m_bpb;
		std::vector<fat> m_fat;
	};
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FAT_DRIVER_HH_
