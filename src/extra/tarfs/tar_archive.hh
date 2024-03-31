//
// Created by igor on 3/18/24.
//

#ifndef VFS_SRC_EXTRA_TARFS_TAR_ARCHIVE_HH_
#define VFS_SRC_EXTRA_TARFS_TAR_ARCHIVE_HH_

#include <iosfwd>
#include <memory>
#include <map>
#include <string>
#include "microtar.h"
#include "extra/extra_tools/archived_fs.hh"

namespace vfs::extra {
	struct tar_entry {
		tar_entry()
		: is_dir(true),
		size(0),
		offset(0){
		}

		bool                             is_dir;
		uint64_t                         size;
		uint64_t                         offset;
		std::string                      name;
	};

	template <>
	struct entry_props_traits<tar_entry> {
		static uint64_t get_archived_size(const tar_entry& e) {
			return e.size;
		}

		static uint64_t get_size(const tar_entry& e) {
			return e.size;
		}

		static std::string get_file_name(const tar_entry& e) {
			return e.name;
		}

		static void update_tree_from_entry(tree_entry<tar_entry>& out, const tar_entry& e) {
			out.is_dir = e.is_dir;
			out.name = e.name;
			out.offset = e.offset;
			out.size = e.size;
		}

		static bool is_dir(const tar_entry& e) {
			return e.is_dir;
		}
	};



	class tar_archive : public archive_io<tar_entry> {
	 public:
		explicit tar_archive (const std::string& params);
		~tar_archive () override;

		std::optional<tar_entry> next_entry() override;
		std::unique_ptr<archive_reader<tar_entry>> create_reader (tar_entry& entry) override;

	 private:
		std::unique_ptr<std::istream> m_stream;
		std::unique_ptr<mtar_t> m_tar;
	};
}

#endif //VFS_SRC_EXTRA_TARFS_TAR_ARCHIVE_HH_
