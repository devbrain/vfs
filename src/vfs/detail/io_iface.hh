//
// Created by igor on 2/4/24.
//

#ifndef VFS_SRC_API_IO_IFACE_HH_
#define VFS_SRC_API_IO_IFACE_HH_

#include "vfs/io.hh"

namespace vfs {
	struct file;

	namespace core {
		class dentry_tree;
		class fstab;

		std::optional<vfs::stats> get_stats (const std::string& pth, const dentry_tree* tree);
		vfs::directory open_directory (const std::string& pth, const dentry_tree* tree);
		void create_directory (const std::string& pth, const dentry_tree* tree);
		void unlink (const std::string& pth, const dentry_tree* tree, fstab* fs_table);

		VFS_API file* open (const std::string& path, creation_disposition cd, bool readonly, const dentry_tree* tree);

		vfs::file* open (const std::string& path, unsigned openmode, const dentry_tree* tree);

		void close (file* f);

		size_t read (file* f, void* buff, size_t len);
		size_t write (file* f, const void* buff, size_t len);
		bool truncate (file* f);


		void seek (file* f, uint64_t pos, seek_type whence);
		uint64_t tell (file* f);

	}
}

#endif //VFS_SRC_API_IO_IFACE_HH_
