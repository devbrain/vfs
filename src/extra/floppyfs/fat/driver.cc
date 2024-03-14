//
// Created by igor on 3/14/24.
//

#include "driver.hh"

namespace vfs::extra {
	driver::driver (std::istream& is)
	: m_istream(is),
	  m_bpb (vfs::extra::read_bpb (is)) {
		for (const auto& loc : m_bpb.fat) {
			m_fat.emplace_back (m_bpb.type, loc, is);
		}
	}

	uint32_t driver::fat_get (uint32_t cluster) const {
		return m_fat[0].get (cluster);
	}

	void driver::fat_set (uint32_t cluster, uint32_t value) {
		for (auto& f : m_fat) {
			f.set (cluster, value);
		}
	}

	void driver::fat_set (uint32_t cluster, fat::cluster_type_t t) {
		for (auto& f : m_fat) {
			f.set (cluster, t);
		}
	}

	uint32_t driver::fat_size () const {
		return m_fat[0].size();
	}

	std::optional<fat::cluster_type_t> driver::get_cluster_type (uint32_t value) const {
		return m_fat[0].get_cluster_type (value);
	}

	static constexpr uint32_t MSDOSFSROOT = 0;
	static constexpr uint32_t CLUST_FIRST = 2;

	std::size_t driver::get_cluster_offset (uint32_t cluster) const {
		std::size_t p = m_bpb.root_directory.offset;
		if (cluster != MSDOSFSROOT) {
			p += m_bpb.root_directory.size;
			p += m_bpb.bytes_per_sector * m_bpb.sectors_per_cluster * (cluster - CLUST_FIRST);
		}
		return p;
	}

	std::istream& driver::stream () {
		return m_istream;
	}

	const std::istream& driver::stream () const {
		return m_istream;
	}

}