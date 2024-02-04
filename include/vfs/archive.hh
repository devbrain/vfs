//
// Created by igor on 2/1/24.
//

#ifndef VFS_INCLUDE_VFS_ARCHIVE_HH_
#define VFS_INCLUDE_VFS_ARCHIVE_HH_

#include <memory>
#include <string>

#include <bsw/spimpl.h>

#include <vfs/api/vfs_api.h>
#include "vfs/api/vfs_module.h"
#include <vfs/io.hh>

namespace vfs {
	class VFS_API archive {
	 public:
		explicit archive(std::unique_ptr<module::filesystem> fsptr, const std::string& args);
		virtual ~archive();

		std::optional<stats> get_stats (const std::string& pth);
		directory open_directory (const std::string& pth);
		void create_directory (const std::string& pth);
		void unlink (const std::string& pth);

		file* open (const std::string& path, unsigned openmode);

		void close (file* f);

		size_t read (file* f, void* buff, size_t len);
		size_t write (file* f, const void* buff, size_t len);
		bool truncate (file* f);


		void seek (file* f, uint64_t pos, seek_type whence);
		uint64_t tell (file* f);

	 private:
		struct impl;
		spimpl::impl_ptr<impl> m_pimpl;
	};

	class VFS_API physfs_archive : public archive {
	 public:
		explicit physfs_archive(const std::string& args);
	};
}

#endif //VFS_INCLUDE_VFS_ARCHIVE_HH_
