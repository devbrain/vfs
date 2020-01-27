#ifndef VFS_API_DIRECTORY_ITERATOR_HH
#define VFS_API_DIRECTORY_ITERATOR_HH


#include <tuple>
#include <string>
#include <memory>

#include <vfs/api/stats.hh>
#include <vfs/api/vfs_api.h>

namespace vfs
{
	// forward declarations
	namespace core
	{
		class inode;
	}
	class VFS_API directory;
	VFS_API  directory open_directory (const std::string& pth);
	// ---------------------------------------------------------------------

	class VFS_API directory
	{
		friend directory open_directory (const std::string& pth);
	public:
		std::tuple<std::string, stats> next ();
		bool has_next() const;

		~directory();
	private:
		explicit directory(core::inode* ino);

		struct impl;
		std::unique_ptr<impl> _pimpl;
	};


	class VFS_API directory_iterator
	{
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::tuple<std::string, stats>;
		using difference_type = std::ptrdiff_t;
		using pointer = const value_type*;
		using reference = const value_type&;

		directory_iterator();

		directory_iterator(directory& directoryToWrap);
		directory_iterator(const directory_iterator& other);
		reference operator*() const;
		pointer operator->() const;
		directory_iterator& operator++();
		directory_iterator operator++(int);
		friend bool operator==(const directory_iterator& lhs, const directory_iterator& rhs);
		friend bool operator!=(const directory_iterator& lhs, const directory_iterator& rhs);

	private:
		void assert_directory() const;

		directory* _directory;
		value_type _ino_stats;
	};

	VFS_API directory_iterator begin(directory& w);
	VFS_API directory_iterator end(directory& w);
}



#endif