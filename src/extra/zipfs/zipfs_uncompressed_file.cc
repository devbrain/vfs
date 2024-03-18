//
// Created by igor on 2/26/24.
//

#include <algorithm>
#include <bsw/exception.hh>
#include "zipfs_uncompressed_file.hh"
#include "zip_archive.hh"

namespace vfs::extra {

	zipfs_uncompressed_file::zipfs_uncompressed_file (zip_archive* archive, const zip_tree* node)
	: m_archive(archive),
	  m_entry_offset(node->offset),
	  m_entry_size(node->compressed_size),
	  m_file_size(node->original_size), // compressed_size == original_size
	  m_pointer (0) {

	}

	ssize_t zipfs_uncompressed_file::read (void* buff, size_t len) {
		auto to_read = std::min(static_cast<size_t>(m_file_size - m_pointer), len);
		if (m_archive->read (buff, to_read, m_pointer + m_entry_offset) != to_read) {
			RAISE_EX("zipfs I/O error");
		}
		m_pointer += to_read;
		return static_cast<ssize_t>(to_read);
	}

	ssize_t zipfs_uncompressed_file::write ([[maybe_unused]] void* buff, [[maybe_unused]] size_t len) {
		RAISE_EX("Write operation is not supported for zipfs");
	}

	bool zipfs_uncompressed_file::seek (uint64_t pos, whence_type whence) {
		if (pos > m_file_size) {
			return false;
		}
		auto new_offset = m_pointer;

		switch (whence) {
			case eVFS_SEEK_CUR:
				new_offset += pos;
				break;
			case eVFS_SEEK_SET:
				new_offset = pos;
				break;
			case eVFS_SEEK_END:
				new_offset = m_file_size - pos;
				break;
		}
		m_pointer = new_offset;
		return true;
	}

	bool zipfs_uncompressed_file::truncate () {
		RAISE_EX("truncate operation is not supported for zipfs");
	}

	uint64_t zipfs_uncompressed_file::tell () const {
		return m_pointer;
	}

}