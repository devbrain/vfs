#include "mount_point.hh"
#include <algorithm>
#include <iostream>

namespace vfs::core
{
	mount_point::mount_point(std::unique_ptr<inode> root)
	{
		std::shared_ptr<inode> sptr (std::move(root));
		_root = std::weak_ptr<inode>(sptr);
		_allocated_nodes.insert(std::move(sptr));
	}
	// ---------------------------------------------------------------------------
	mount_point::~mount_point()
	{
		std::for_each(_allocated_nodes.begin(), _allocated_nodes.end(),
			[this](const std::shared_ptr<inode>& x) -> void
			{
				if (x.use_count() > 1)
				{
					std::cout << "(mount point) MEMORY LEAK" << std::endl;
				}
			}
			);
	//	std::cout << "mount_point::~mount_point()";
	}
	// ---------------------------------------------------------------------------
	wrapped_pointer<inode> mount_point::root()  const noexcept
	{
		return wrapped_pointer<inode>(_root.lock().get());
	}
	// ---------------------------------------------------------------------------
	wrapped_pointer<inode> mount_point::add(std::unique_ptr<inode> ino)
	{
		std::shared_ptr<inode> sptr (std::move(ino));
		std::weak_ptr<inode> ret(sptr);
		_allocated_nodes.insert(std::move(sptr));
		return wrapped_pointer<inode>(ret.lock().get());
	}
	// ---------------------------------------------------------------------------
	void mount_point::remove(inode* ino)
	{
		uintptr_t key = reinterpret_cast<uintptr_t>(ino);
		auto itr = _allocated_nodes.find(key);
		if (itr != _allocated_nodes.end())
		{
			_allocated_nodes.erase(itr);
		}
	}
} // ns vfs::core
