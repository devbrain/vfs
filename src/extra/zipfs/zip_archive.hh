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

#include <extra/extra_tools/archived_fs.hh>

#include "miniz.h"

namespace vfs::extra {

	struct zip_entry {
		zip_entry()
		: is_dir(true),
		is_compressed(false),
		original_size(0),
		compressed_size(0),
		offset(0) {}

		bool                             is_dir;
		bool 							 is_compressed;
		std::string                      comments;
		uint64_t                         original_size;
		uint64_t                         compressed_size;
		uint64_t                         offset;
		std::string 					 name;
	};

	template <>
	struct entry_props_traits<zip_entry> {
		static uint64_t get_archived_size(const zip_entry& e) {
			return e.compressed_size;
		}

		static uint64_t get_size(const zip_entry& e) {
			return e.original_size;
		}

		static std::string get_file_name(const zip_entry& e) {
			return e.name;
		}

		static void update_tree_from_entry(tree_entry<zip_entry>& out, const zip_entry& e) {
			out.is_dir = e.is_dir;
			out.is_compressed = e.is_compressed;
			out.comments = e.comments;
			out.original_size = e.original_size;
			out.compressed_size = e.compressed_size;
			out.offset = e.offset;
			out.name = e.name;
		}

		static bool is_dir(const zip_entry& e) {
			return e.is_dir;
		}
	};

	class zip_archive : public archive_io<zip_entry> {
	 public:
		zip_archive(std::unique_ptr<std::istream> istream, uint64_t file_size);
		~zip_archive() override;

		std::optional<zip_entry> next_entry() override;
		std::unique_ptr<archive_reader<zip_entry>> create_reader (zip_entry& entry) override;
	 private:
		std::unique_ptr<std::istream> m_istream;
		uint64_t                      m_file_size;
		std::size_t                   m_num_files;
		std::size_t                   m_curr_file;
	 	mz_zip_archive                m_archive;
	};
}

#endif //VFS_SRC_EXTRA_ZIPFS_ZIP_ARCHIVE_HH_
