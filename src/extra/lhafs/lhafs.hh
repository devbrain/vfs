//
// Created by igor on 3/28/24.
//

#ifndef VFS_SRC_EXTRA_LHAFS_LHAFS_HH_
#define VFS_SRC_EXTRA_LHAFS_LHAFS_HH_

#include <iosfwd>
#include <vfs/extra/lhafs.hh>
#include "extra/extra_tools/archived_fs.hh"

#include "lha_archive.hh"

namespace vfs::extra {
	class lhafs : public archived_fs<lha_entry> {
	 public:
		lhafs();
		~lhafs() override;
	 private:
		std::unique_ptr<archive_io<lha_entry>> create_archive_io (const std::string& params) override;
	 private:
		std::istream* m_stream;
	};
}

#endif //VFS_SRC_EXTRA_LHAFS_LHAFS_HH_
