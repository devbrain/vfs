//
// Created by igor on 3/28/24.
//

#include <fstream>
#include "lhafs.hh"


namespace vfs::extra {

	lhafs::lhafs ()
		: archived_fs<lha_entry> ("lhafs"),
		  m_stream (nullptr) {

	}

	lhafs::~lhafs () {
		delete m_stream;
	}

	std::unique_ptr<archive_io<lha_entry>> lhafs::create_archive_io (const std::string& params) {
		m_stream = new std::ifstream (params, std::ios::in | std::ios::binary);
		return std::make_unique<lha_archive>(m_stream);
	}

	std::unique_ptr<vfs::module::filesystem> create_lhafs() {
		return std::make_unique<lhafs>();
	}
}

#if !defined(LHAFS_STATIC_DEFINE)
REGISTER_VFS_MODULE(vfs::extra::lhafs)
#endif