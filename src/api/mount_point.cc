#include "mount_point.hh"
#include <algorithm>
#include <iostream>

namespace vfs::core
{
	mount_point::mount_point(std::unique_ptr<inode> root)
	{
		_allocated_nodes.insert(std::move(root));
	}
	// ---------------------------------------------------------------------------
	mount_point::~mount_point()
	{
		std::for_each(_allocated_nodes.begin(), _allocated_nodes.end(),
			[this](const std::shared_ptr<inode>& x) -> void
			{
				if (x.use_count() > 1)
				{
					std::cout << "MEMORY LEAK" << std::endl;
				}
			}
			);
		std::cout << "mount_point::~mount_point()";
	}
} // ns vfs::core
