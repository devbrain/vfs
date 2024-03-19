//
// Created by igor on 3/18/24.
//

#ifndef VFS_SRC_EXTRA_TARFS_TARFS_FILE_HH_
#define VFS_SRC_EXTRA_TARFS_TARFS_FILE_HH_
#include <vfs/api/vfs_module.h>
#include "tar_archive.hh"
namespace vfs::extra {
	class tarfs_file : public vfs::module::file {
	 public:
		tarfs_file (tar_archive* archive, const tar_tree* node);
		~tarfs_file () override = default;
	 protected:
		ssize_t read (void* buff, size_t len) override;
		ssize_t write (void* buff, size_t len) override;
		bool seek (uint64_t pos, whence_type whence) override;
		bool truncate () override;
		[[nodiscard]] uint64_t tell () const override;
	 protected:
		tar_archive* m_archive;
		const uint64_t     m_entry_offset;
		const uint64_t     m_entry_size;
		uint64_t           m_pointer;
	};
}


#endif //VFS_SRC_EXTRA_TARFS_TARFS_FILE_HH_
