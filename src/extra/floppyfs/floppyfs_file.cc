//
// Created by igor on 3/10/24.
//

#include "floppyfs_file.hh"

namespace vfs::extra {
	floppyfs_file::floppyfs_file(driver* fat, uint32_t start_cluster, std::size_t file_size)
	: m_fat(fat),
	  m_start_cluster (start_cluster),
	  m_current_cluster (0),
	  m_file_size(file_size),
	  m_bytes_per_cluster(fat->get_bytes_per_cluster()),
	  m_pointer(0),
	  m_file_map(fat->get_file_map (start_cluster, file_size)) {
	}

	floppyfs_file::~floppyfs_file() = default;

	ssize_t floppyfs_file::read (void* buff, size_t len) {
		char* out = (char*)buff;
		auto max_bytes = m_file_size - m_pointer;
		std::size_t has_bytes = 0;
		while (has_bytes < len && has_bytes < max_bytes) {
			auto rc = read_cluster (out + has_bytes, len - has_bytes);
			if (rc == -1) {
				return -1;
			}
			has_bytes += rc;
		}

		return 0;
	}

	ssize_t floppyfs_file::read_cluster (void* buff, size_t len) {
		auto offs = m_file_map[m_current_cluster];
		auto in_cluster_offs = m_pointer % m_bytes_per_cluster;
		auto has_bytes = offs.size - in_cluster_offs;
		auto to_read = std::min(has_bytes, len);
		m_fat->stream().seekg (offs.offset, std::ios::beg);
		m_fat->stream().read ((char*)buff, to_read);
		if (!m_fat->stream()) {
			return -1;
		}
		seek (to_read, eVFS_SEEK_CUR);
		return to_read;
	}

	ssize_t floppyfs_file::write (void* buff, size_t len) {
		return 0;
	}

	bool floppyfs_file::seek (uint64_t pos, whence_type whence) {
		bool ok = false;
		uint64_t current_pointer = m_pointer;
		if (whence == eVFS_SEEK_SET) {
			if (pos < m_file_size) {
				m_pointer = pos;
				ok = true;
			}
		} else if (whence == eVFS_SEEK_CUR) {
			if (m_pointer + pos < m_file_size) {
				m_pointer += pos;
				ok = true;
			}
		} else if (whence == eVFS_SEEK_END) {
			if (pos <= m_file_size) {
				m_pointer = m_file_size - pos;
				ok = true;
			}
		}
		if (ok) {
			m_current_cluster = current_pointer / m_bytes_per_cluster;
		}
		return ok;
	}

	bool floppyfs_file::truncate () {
		return false;
	}

	uint64_t floppyfs_file::tell () const {
		return m_pointer;
	}
}