//
// Created by igor on 3/12/24.
//

#ifndef VFS_SRC_EXTRA_FLOPPYFS_FAT_FAT_HH_
#define VFS_SRC_EXTRA_FLOPPYFS_FAT_FAT_HH_

#include <istream>
#include <ostream>
#include <cstdint>
#include <vector>
#include <optional>

#include "fat/bpb.hh"

namespace vfs::extra {
	class fat {
	 public:
		enum cluster_type_t {
			FREE,
			MUST_NOT_BE_USED,
			DEFECTIVE,
			SHOULD_NOT_BE_USED,
			FINAL
		};
	 public:
		fat(bpb::type_t type, const disk_entry& location, std::istream& is);
		[[nodiscard]] uint32_t get(uint32_t cluster) const;
		void set(uint32_t cluster, uint32_t value);
		void set(uint32_t cluster, cluster_type_t t);
		[[nodiscard]] uint32_t size() const;
		[[nodiscard]] std::optional<cluster_type_t> get_cluster_type(uint32_t value) const;
	 private:
		bpb::type_t           m_type;
		const disk_entry      m_location;
		std::vector<char>     m_fat;
		std::size_t           m_num_of_clusters;
	};
}

#endif //VFS_SRC_EXTRA_FLOPPYFS_FAT_FAT_HH_
