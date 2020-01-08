#ifndef VFS_API_MOUNT_POINT_HH
#define VFS_API_MOUNT_POINT_HH

#include "api/filesystem.hh"
#include <memory>
#include <set>

namespace vfs::core
{
	class mount_point
	{
	public:
		using handle_t = uintptr_t;
	public:
		explicit mount_point(std::unique_ptr<inode> root);

		~mount_point();
	private:
		struct compare
		{
			using is_transparent = int; // C++14 transparent comparator

			bool operator() (const std::shared_ptr<inode>& a, const std::shared_ptr<inode>& b) const noexcept
			{
				return reinterpret_cast<uintptr_t>(a.get()) < reinterpret_cast<uintptr_t>(b.get());
			}

			bool operator() (const std::shared_ptr<inode>& a, uintptr_t b) const noexcept
			{
				return reinterpret_cast<uintptr_t>(a.get()) < b;
			}

			bool operator() (uintptr_t a, const std::shared_ptr<inode>& b) const noexcept
			{
				return a < reinterpret_cast<uintptr_t>(b.get());
			}
		};
		std::set<std::shared_ptr<inode>, compare> _allocated_nodes;
	};
} // ns vfs::core


#endif
