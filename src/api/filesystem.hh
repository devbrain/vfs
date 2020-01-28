#ifndef VFS_FILESYSTEM_HH
#define VFS_FILESYSTEM_HH

#include "vfs/api/vfs_module.h"

#include <string>
#include <memory>


namespace vfs::core
{
	class inode;
	class directory_iterator;
	class modules_table;

	class filesystem
	{
		friend class modules_table;
	public:
		explicit filesystem(vfs_module* ops);

		filesystem(const filesystem&) = delete;
		filesystem& operator = (const filesystem&) = delete;



		[[nodiscard]] std::unique_ptr<inode> load_root(const std::string& params);
		[[nodiscard]] size_t max_name_length() const noexcept ;
		[[nodiscard]] std::string type() const noexcept ;
	private:
		~filesystem();
		vfs_module* _module;
	};
	// -------------------------------------------------------------
	struct stats : public vfs_inode_stats
	{
	public:
		stats();

		stats(const stats&) = delete;
		stats& operator = (const stats&) = delete;

		~stats();
	};
	// -------------------------------------------------------------
	class directory_iterator
	{
		friend class inode;
	public:
		directory_iterator(const directory_iterator&) = delete;
		directory_iterator& operator = (const directory_iterator&) = delete;

		~directory_iterator();
		[[nodiscard]] bool has_next() const;
		std::string next();

		[[nodiscard]] const inode* owner() const;
	private:
		explicit directory_iterator(vfs_directory_iterator* ops, const inode* owner);
	private:
		vfs_directory_iterator* _ops;
		const inode* _owner;

	};
	// -------------------------------------------------------------
	class inode
	{
		friend class filesystem;
	public:
		inode(const inode&) = delete;
		inode& operator = (const inode&) = delete;

		~inode();

		[[nodiscard]] std::unique_ptr<inode> lookup(const std::string& name) const;
		void stat(stats& st) const;
		[[nodiscard]] std::unique_ptr<directory_iterator> get_directory_iterator() const;

		[[nodiscard]] const filesystem* owner() const;
		[[nodiscard]] bool mkdir(const std::string& name);
		[[nodiscard]] bool dirty() const noexcept ;
	private:
		explicit inode(vfs_inode_ops* ops, filesystem* owner);
	private:
		vfs_inode_ops* _ops;
		filesystem* _owner;
		bool _dirty;
	};
}

#endif
