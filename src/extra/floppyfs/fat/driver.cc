//
// Created by igor on 3/14/24.
//

#include <bsw/exception.hh>
#include "driver.hh"

namespace vfs::extra {
	driver::driver (std::istream& is)
		: m_istream (is),
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
		return m_fat[0].size ();
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

	directory driver::get_root_directory () {
		return {*this, m_bpb.root_dir_entries};
	}

	directory driver::get_directory (uint32_t cluster) {
		if (cluster == 0) {
			return get_root_directory ();
		}
		return {cluster, m_bpb.sectors_per_cluster * m_bpb.bytes_per_sector, *this};
	}

	std::size_t driver::get_bytes_per_cluster () const {
		return m_bpb.bytes_per_sector * m_bpb.sectors_per_cluster;
	}

	std::vector<driver::file_map> driver::get_file_map (uint32_t cluster, std::size_t size_of_file) const {
		std::vector<file_map> out;
		get_file_map_helper (cluster, size_of_file, out);
		return out;
	}

	void driver::get_file_map_helper (uint32_t cluster, uint32_t bytes_remaining, std::vector<file_map>& out) const {
		uint32_t clust_size = get_bytes_per_cluster ();
		auto total_clusters = m_bpb.count_of_clusters;
		if (cluster == 0) {
			RAISE_EX("Bad file termination");
		} else {
			auto ct = get_cluster_type(cluster);
			if (ct && *ct == fat::FINAL) {
				return;
			} else if (cluster > total_clusters) {
				RAISE_EX("Too many clusters");
			}
		}

		/* map the cluster number to the data location */
		auto offset = get_cluster_offset (cluster);

		if (bytes_remaining <= clust_size) {
			out.emplace_back (offset, bytes_remaining);
		} else {
			/* more clusters after this one */
			out.emplace_back (offset, clust_size);
			/* recurse, continuing to copy */
			get_file_map_helper (fat_get (cluster), bytes_remaining - clust_size, out);
		}
	}

	driver::file_map::file_map (size_t offset, uint16_t size)
		: offset (offset), size (size) {
	}
}