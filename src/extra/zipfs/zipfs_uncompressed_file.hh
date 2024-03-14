//
// Created by igor on 2/26/24.
//

#ifndef VFS_SRC_EXTRA_ZIPFS_ZIPFS_UNCOMPRESSED_FILE_HH_
#define VFS_SRC_EXTRA_ZIPFS_ZIPFS_UNCOMPRESSED_FILE_HH_

#include <vfs/api/vfs_module.h>
#include "zip_archive.hh"

namespace vfs::extra {
	class zip_archive;
	struct zip_tree;

	class zipfs_uncompressed_file : public vfs::module::file {
	 public:
		zipfs_uncompressed_file (zip_archive* archive, const zip_tree* node);


		~zipfs_uncompressed_file () override = default;
	 protected:
		ssize_t read (void* buff, size_t len) override;
		ssize_t write (void* buff, size_t len) override;
		bool seek ([[maybe_unused]] uint64_t pos, [[maybe_unused]] whence_type whence) override;
		bool truncate () override;
		[[nodiscard]] uint64_t tell () const override;
	 protected:
		const zip_archive* m_archive;
		const uint64_t     m_entry_offset;
		const uint64_t     m_entry_size;
		const uint64_t     m_file_size;
		uint64_t           m_pointer;
	};
}

#endif //VFS_SRC_EXTRA_ZIPFS_ZIPFS_UNCOMPRESSED_FILE_HH_
