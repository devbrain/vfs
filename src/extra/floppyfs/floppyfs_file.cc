//
// Created by igor on 3/10/24.
//

#include "floppyfs_file.hh"

namespace vfs::extra {
	floppyfs_file::floppyfs_file(driver* fat, uint32_t start_cluster, std::size_t file_size)
	: m_fat(fat),
	  m_start_cluster (start_cluster),
	  m_current_cluster (start_cluster),
	  m_file_size(file_size),
	  m_bytes_per_cluster(fat->get_bytes_per_cluster()),
	  m_pointer(0) {
	}

	floppyfs_file::~floppyfs_file() = default;

	ssize_t floppyfs_file::read (void* buff, size_t len) {

		return 0;
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
			auto current_cluster = current_pointer / m_bytes_per_cluster;
			auto new_cluster = m_pointer / m_bytes_per_cluster;
			if (new_cluster != current_cluster) {

			}
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