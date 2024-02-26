//
// Created by igor on 2/26/24.
//

#ifndef VFS_SRC_EXTRA_ZIPFS_ZIPFS_INMEM_FILE_HH_
#define VFS_SRC_EXTRA_ZIPFS_ZIPFS_INMEM_FILE_HH_

#include <vector>
#include "zipfs_uncompressed_file.hh"

namespace vfs::extra {
	class zipfs_inmem_file : public zipfs_uncompressed_file {
	 public:
		zipfs_inmem_file(zip_archive* archive, const zip_tree* node);
	 protected:
		ssize_t read (void* buff, size_t len) override;
	 private:
		std::vector<char> m_data;
	};
}
#endif //VFS_SRC_EXTRA_ZIPFS_ZIPFS_INMEM_FILE_HH_
