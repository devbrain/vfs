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

namespace vfs::extra {
	struct tar_tree {
		tar_tree();
		~tar_tree();

		bool                             is_dir;
		uint64_t                         size;
		uint64_t                         offset;

		using children_t = std::map<std::string, tar_tree*>;
		children_t children;
	};

	class tar_archive {
	 public:
		explicit tar_archive (std::istream& is);
		~tar_archive ();
		std::istream& stream();
		[[nodiscard]] const std::istream& stream() const;

		tar_tree* get_root();
		[[nodiscard]] const tar_tree* get_root() const;
	 private:
		void add_to_tree (const mtar_header_t& h);

	 private:
		std::istream& m_stream;
		std::unique_ptr<mtar_t> m_tar;
		tar_tree* m_root;
	};
}

#endif //VFS_SRC_EXTRA_TARFS_TAR_ARCHIVE_HH_
