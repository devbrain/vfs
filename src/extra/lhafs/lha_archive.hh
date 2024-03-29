//
// Created by igor on 3/19/24.
//

#ifndef VFS_SRC_EXTRA_LHAFS_LHA_ARCHIVE_HH_
#define VFS_SRC_EXTRA_LHAFS_LHA_ARCHIVE_HH_

#include <iosfwd>
#include <string>
#include <memory>
#include <cstdint>

#include "extra/extra_tools/archived_fs.hh"

namespace vfs::extra {

	struct lha_entry {
		lha_entry()
		: is_file(false),
		data_pos(0),
		compressed_size(0),
		decompressed_size(0) {}

		lha_entry (bool is_file,
				   std::string  name,
				   uint64_t data_pos,
				   size_t compressed_size,
				   size_t decompressed_size,
				   const char* compression_method);
		bool is_file;
		std::string name;
		uint64_t data_pos;
		std::size_t compressed_size;
		std::size_t decompressed_size;
		std::string compression_method;
	};

	template <>
	struct entry_props_traits<lha_entry> {
		static uint64_t get_archived_size(const lha_entry& e) {
			return e.compressed_size;
		}

		static uint64_t get_size(const lha_entry& e) {
			return e.decompressed_size;
		}

		static std::string get_file_name(const lha_entry& e) {
			return e.name;
		}

		static void update_tree_from_entry(tree_entry<lha_entry>& out, const lha_entry& e) {
			out.is_file = e.is_file;
			out.name = e.name;
			out.data_pos = e.data_pos;
			out.compressed_size = e.compressed_size;
			out.decompressed_size = e.decompressed_size;
			out.compression_method = e.compression_method;
		}

		static bool is_dir(const lha_entry& e) {
			return !e.is_file;
		}
	};


	class lha_archive : public archive_io<lha_entry> {
	 public:
		explicit lha_archive(std::istream* is);
		~lha_archive() override;
	 private:
		std::optional<lha_entry> next_entry() override;
		std::unique_ptr<archive_reader<lha_entry>> create_reader (lha_entry& entry) override;
	 private:
		class entry_reader_impl;
		std::istream* m_stream;
		std::unique_ptr<entry_reader_impl> m_reader;

	};

}

#endif //VFS_SRC_EXTRA_LHAFS_LHA_ARCHIVE_HH_
