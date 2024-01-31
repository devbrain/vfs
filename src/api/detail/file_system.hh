#ifndef VFS_FILESYSTEM_HH
#define VFS_FILESYSTEM_HH

#include "vfs/api/vfs_module.h"

#include <string>
#include <memory>

namespace vfs::core {
	class inode;

	class directory_iterator;

	class modules_table;

	class file_system {
		friend class modules_table;

	 public:
		explicit file_system (vfs_module* ops);

		file_system (const file_system&) = delete;
		file_system& operator= (const file_system&) = delete;

		[[nodiscard]] std::unique_ptr<inode> load_root (const std::string& params);
		[[nodiscard]] size_t max_name_length () const noexcept;
		[[nodiscard]] std::string type () const noexcept;
		[[nodiscard]] std::string description () const noexcept;
		[[nodiscard]] int sync ();
		[[nodiscard]] int sync (inode* ino);

		[[nodiscard]] bool is_readonly () const;
	 private:
		~file_system ();
		vfs_module* _module;
		bool _is_readonly;
	};

	// -------------------------------------------------------------
	struct stats : public vfs_inode_stats {
	 public:
		stats ();

		stats (const stats&) = delete;
		stats& operator= (const stats&) = delete;

		~stats ();
	};

	// -------------------------------------------------------------
	class directory_iterator {
		friend class inode;

	 public:
		directory_iterator (const directory_iterator&) = delete;
		directory_iterator& operator= (const directory_iterator&) = delete;

		~directory_iterator ();
		[[nodiscard]] bool has_next () const;
		std::string next ();

		[[nodiscard]] const inode* owner () const;
	 private:
		directory_iterator (vfs_directory_iterator* ops, const inode* owner);
	 private:
		vfs_directory_iterator* _ops;
		const inode* _owner;
	};

	// -------------------------------------------------------------
	class file_ops {
		friend class inode;

	 public:
		[[nodiscard]] bool seek (uint64_t pos, enum whence_type whence);
		[[nodiscard]] uint64_t tell () const;
		[[nodiscard]] ssize_t read (void* buff, size_t len);
		[[nodiscard]] ssize_t write (const void* buff, size_t len);
		[[nodiscard]] bool truncate ();
		~file_ops ();
		[[nodiscard]] const inode* owner () const;
		[[nodiscard]] bool is_readonly () const;
	 private:
		file_ops (vfs_file_ops* ops, inode* owner);
	 private:
		vfs_file_ops* _ops;
		inode* _owner;
	};

	// -------------------------------------------------------------
	class inode {
		friend class file_system;

		friend class file_ops;

	 public:
		inode (const inode&) = delete;
		inode& operator= (const inode&) = delete;

		~inode ();

		[[nodiscard]] std::unique_ptr<inode> lookup (const std::string& name) const;
		void stat (stats& st) const;
		[[nodiscard]] std::unique_ptr<directory_iterator> get_directory_iterator () const;

		[[nodiscard]] const file_system* owner () const;
		[[nodiscard]] bool mkdir (const std::string& name);
		[[nodiscard]] bool mkfile (const std::string& name);
		[[nodiscard]] bool dirty () const noexcept;
		[[nodiscard]] int sync ();
		[[nodiscard]] int unlink ();

		[[nodiscard]] std::unique_ptr<file_ops> get_file_ops (open_mode_type mode_type) const;
		[[nodiscard]] bool is_readonly () const;
	 private:
		explicit inode (vfs_inode_ops* ops, file_system* owner);
	 private:
		void _make_dirty ();
	 private:
		vfs_inode_ops* _ops;
		file_system* _owner;
		bool _dirty;
	};
}

#endif