//
// Created by igor on 2/18/24.
//

#ifndef VFS_SRC_EXTRA_ZIPFS_ZIP_ARCHIVE_HH_
#define VFS_SRC_EXTRA_ZIPFS_ZIP_ARCHIVE_HH_

#include <iosfwd>
#include <cstdint>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include "miniz.h"

namespace vfs::extra {

	struct zip_tree {
		zip_tree();
		~zip_tree();

		bool                             is_dir;
		bool 							 is_compressed;
		std::string                      comments;
		uint64_t                         original_size;
		uint64_t                         compressed_size;
		uint64_t                         offset;

		using children_t = std::map<std::string, zip_tree*>;
		std::map<std::string, zip_tree*> children;
	};

	class zip_archive {
	 public:
		zip_archive(std::unique_ptr<std::istream> istream, uint64_t file_size);
		~zip_archive();
		zip_tree* get_root();

		[[nodiscard]] size_t read(void* buf, size_t len, uint64_t offset) const;
		uint64_t decompress(std::vector<char>& out, uint64_t entry_offset, uint64_t entry_size) const;
	 private:
		void add_to_tree (const mz_zip_archive_file_stat& s);
		[[nodiscard]] uint64_t get_data_offset(const mz_zip_archive_file_stat& s) const;
	 private:
		std::unique_ptr<std::istream> m_istream;
	 	mz_zip_archive m_archive;
		zip_tree* m_root;
	};
}

#endif //VFS_SRC_EXTRA_ZIPFS_ZIP_ARCHIVE_HH_
