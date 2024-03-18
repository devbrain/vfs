//
// Created by igor on 3/18/24.
//

#include <istream>
#include <bsw/exception.hh>
#include "tarfs_file.hh"


namespace vfs::extra {

	tarfs_file::tarfs_file (tarfile* archive, const tar_tree* node)
	:m_archive(archive),
	m_entry_offset(node->offset),
	m_entry_size(node->size),
	m_pointer (0) {

	}
	ssize_t tarfs_file::read (void* buff, size_t len) {
		auto to_read = std::min(static_cast<size_t>(m_entry_size - m_pointer), len);
		auto & stream = m_archive->stream();
		stream.seekg (m_pointer + m_entry_offset, std::ios::beg);
		if (!m_archive->stream().read ((char*)buff, to_read)) {
			RAISE_EX("zipfs I/O error");
		}
		m_pointer += to_read;
		return static_cast<ssize_t>(to_read);
	}

	ssize_t tarfs_file::write ([[maybe_unused]] void* buff, [[maybe_unused]] size_t len) {
		RAISE_EX("Write operation is not supported for tarfs");
	}

	bool tarfs_file::seek (uint64_t pos, whence_type whence) {
		if (pos > m_entry_size) {
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
				new_offset = m_entry_size - pos;
				break;
		}
		m_pointer = new_offset;
		return true;
	}

	uint64_t tarfs_file::tell () const {
		return m_pointer;
	}

	bool tarfs_file::truncate () {
		RAISE_EX("truncate operation is not supported for tarfs");
	}
}