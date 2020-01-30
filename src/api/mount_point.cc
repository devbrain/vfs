#include "mount_point.hh"
#include <algorithm>
#include <iostream>

namespace vfs::core
{
	mount_point::mount_point(std::unique_ptr<inode> root)
	{
		auto destructor = [this] (inode* pino) {
			this->_inode_destructor(pino);
		};
		std::shared_ptr<inode> sptr (root.release(), destructor);

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
	std::weak_ptr<inode> mount_point::root()  const noexcept
	{
		return _root;
	}
	// ---------------------------------------------------------------------------
	std::weak_ptr<inode> mount_point::add(std::unique_ptr<inode> ino)
	{
		auto destructor = [this] (inode* pino) {
			this->_inode_destructor(pino);
		};
		std::shared_ptr<inode> sptr (ino.release(), destructor);

		std::weak_ptr<inode> ret(sptr);
		_allocated_nodes.insert(std::move(sptr));
		return ret;
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
	// ---------------------------------------------------------------------------
	void mount_point::_inode_destructor(inode* ino) noexcept
	{
		if (ino->dirty())
		{
			auto err = ino->sync();
			if (err != 1)
			{
				std::cout << "ERROR SYNC " << err << std::endl;
			}
		}
		delete ino;
	}
} // ns vfs::core
