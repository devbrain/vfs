//
// Created by igor on 3/10/24.
//

#include "floppyfs_file.hh"

namespace vfs::extra {
	floppyfs_file::floppyfs_file(fat12* fat, uint16_t start_cluster)
	: m_fat(fat),
	  m_start_cluster (start_cluster),
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
		return false;
	}

	bool floppyfs_file::truncate () {
		return false;
	}

	uint64_t floppyfs_file::tell () const {
		return m_pointer;
	}
}