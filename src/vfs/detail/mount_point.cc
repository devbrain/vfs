#include <algorithm>
#include "mount_point.hh"
#include <bsw/logger/logger.hh>

namespace vfs::core {
	mount_point::mount_point (std::unique_ptr<inode> root) {
		auto destructor = [this] (inode* pino) {
		  this->_inode_destructor (pino);
		};
		std::shared_ptr<inode> sptr (root.release (), destructor);

		_root = std::weak_ptr<inode> (sptr);
		_allocated_nodes.insert (std::move (sptr));
	}

	// ---------------------------------------------------------------------------
	mount_point::~mount_point () {
		std::for_each (_allocated_nodes.begin (), _allocated_nodes.end (),
					   [] (const std::shared_ptr<inode>& x) -> void {
						 if (x.use_count () > 1) {
							 EVLOG_TRACE(EVLOG_ERROR, "Memory leak");
						 }
					   }
		);
	}

	// ---------------------------------------------------------------------------
	std::weak_ptr<inode> mount_point::root () const noexcept {
		return _root;
	}

	// ---------------------------------------------------------------------------
	std::weak_ptr<inode> mount_point::add (std::unique_ptr<inode> ino) {
		auto destructor = [this] (inode* pino) {
		  this->_inode_destructor (pino);
		};
		std::shared_ptr<inode> sptr (ino.release (), destructor);

		std::weak_ptr<inode> ret (sptr);
		_allocated_nodes.insert (std::move (sptr));
		return ret;
	}

	// ---------------------------------------------------------------------------
	void mount_point::remove (inode* ino) {
		auto key = reinterpret_cast<uintptr_t>(ino);
		auto itr = _allocated_nodes.find (key);
		if (itr != _allocated_nodes.end ()) {
			_allocated_nodes.erase (itr);
		}
	}

	// ---------------------------------------------------------------------------
	void mount_point::_inode_destructor (inode* ino) noexcept {
		if (ino->dirty ()) {
			auto err = ino->sync ();
			if (err != 1) {
				EVLOG_TRACE(EVLOG_ERROR, "Error syncing inode");
			}
		}
		delete ino;
	}
} // ns vfs::core
