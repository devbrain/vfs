//
// Created by igor on 3/28/24.
//

#ifndef VFS_SRC_EXTRA_EXTRA_TOOLS_READ_ONLY_FS_HH_
#define VFS_SRC_EXTRA_EXTRA_TOOLS_READ_ONLY_FS_HH_

#include "vfs/api/vfs_module.h"

namespace vfs::extra {

	class read_only_file : public vfs::module::file {
	 public:
		read_only_file () = default;
		~read_only_file () override = default;
	 protected:
		ssize_t write ([[maybe_unused]] void* buff, [[maybe_unused]] size_t len) final {
			return -1;
		};
		bool truncate () final {
			return false;
		}
	};

	class read_only_sequential_file : public read_only_file {
	 private:
		bool seek ([[maybe_unused]] uint64_t pos, [[maybe_unused]] whence_type whence) final {
			return false;
		}

		[[nodiscard]] uint64_t tell () const override {
			return 0;
		}
	};

	class readonly_inode : public vfs::module::inode {
	 public:
		readonly_inode (bool is_dir, bool is_sequential)
			: vfs::module::inode (is_dir ? VFS_INODE_DIRECTORY : VFS_INODE_REGULAR),
			  m_is_sequential (is_sequential) {}

	 protected:
		virtual readonly_inode* do_lookup (const char* name) = 0;
		virtual read_only_file* get_readonly_file() = 0;
	 private:
		[[nodiscard]] vfs::module::inode* lookup (const char* name) final {
			return do_lookup (name);
		}

		[[nodiscard]] bool mkdir ([[maybe_unused]] const char* name) final {
			return false;
		}

		[[nodiscard]] bool mkfile ([[maybe_unused]] const char* name) final {
			return false;
		}

		[[nodiscard]] int unlink () final {
			return 0;
		}

		[[nodiscard]] bool is_sequential () const override {
			return m_is_sequential;
		}
		vfs::module::file* open_file (open_mode_type mode_type) final {
			if (mode_type == eVFS_OPEN_MODE_READ) {
				return get_readonly_file();
			}
			return nullptr;
		}
	 private:
		bool m_is_sequential;
	};

	class readonly_fs : public vfs::module::filesystem {
	 public:
		explicit readonly_fs (const char* fsname, size_t max_name_len = 256)
			: vfs::module::filesystem (fsname),
			  m_max_name_len (max_name_len) {}

	 protected:
		virtual readonly_inode* do_load_root (const std::string& params) = 0;
	 private:
		vfs::module::inode* load_root (const std::string& params) final {
			return do_load_root (params);
		}

		size_t max_name_length () final {
			return m_max_name_len;
		}

		int sync () final {
			return 1;
		}

		int sync_inode ([[maybe_unused]] vfs::module::inode* inod) final {
			return 1;
		}

		[[nodiscard]] bool is_readonly () const final {
			return true;
		}

	 private:
		size_t m_max_name_len;
	};
}
#endif //VFS_SRC_EXTRA_EXTRA_TOOLS_READ_ONLY_FS_HH_
