//
// Created by igor on 2/26/24.
//
#include <cstring>
#include "zipfs_inmem_file.hh"
#include "zip_archive.hh"

namespace vfs::extra {

	zipfs_inmem_file::zipfs_inmem_file (zip_archive* archive, const zip_tree* node)
		: zipfs_uncompressed_file (archive, node) {
	}

	ssize_t zipfs_inmem_file::read (void* buff, size_t len) {
		auto to_read = std::min(static_cast<size_t>(m_file_size - m_pointer), len);
		if (len == 0 || m_file_size == 0 || to_read == 0) {
			return 0;
		}
		if (m_data.empty()) {
			m_data.resize (m_file_size);
			m_archive->decompress (m_data, m_entry_offset, m_entry_size);
		}

		std::memcpy (buff, m_data.data() + m_pointer, to_read);
		m_pointer += to_read;
		return static_cast<ssize_t>(to_read);
	}

}