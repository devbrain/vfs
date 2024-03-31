//
// Created by igor on 2/4/24.
//

#include <fstream>

#include <vfs/extra/zipfs.hh>
#include "zipfs_impl.hh"

namespace vfs::extra {
	zipfs::zipfs ()
		: archived_fs<zip_entry> ("zipfs", MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE) {
	}

	std::unique_ptr<archive_io<zip_entry>> zipfs::create_archive_io (const std::string& params) {
		auto stream = std::make_unique<std::ifstream> (params, std::ios::binary | std::ios::in);
		stream->seekg (0, std::ios::end);
		auto size = stream->tellg();
		stream->seekg (0, std::ios::beg);
		return std::make_unique<zip_archive>(std::move (stream), size);
	}

	std::unique_ptr<vfs::module::filesystem> create_zipfs() {
		return std::make_unique<zipfs>();
	}
}

#if !defined(ZIPFS_STATIC_DEFINE)
REGISTER_VFS_MODULE(vfs::extra::zipfs)
#endif

