#include <vfs/exception.hh>
#include "detail/path.hh"
#include "mount_point.hh"
#include "dentry.hh"

#include <bsw/errors.hh>

namespace vfs::core {
	struct dentry {
		dentry (dentry* dparent, wrapped_pointer<mount_point> mp, std::weak_ptr<inode> pino)
			: parent (dparent),
			  mount (mp),
			  ino (std::move (pino)),
			  children(),
			  is_mount (false) {

		}

		void release () {
			if (mount) {
				if (!ino.expired ()) {
					mount->remove (ino.lock ().get ());
				}
			}
			for (const auto& kv : children) {
				delete kv.second;
			}
		}

		~dentry () {
			release ();
		}

		dentry* parent;
		wrapped_pointer<mount_point> mount;
		std::weak_ptr<inode> ino;

		std::map<std::string, dentry*> children;
		bool is_mount;
	};


	dentry_tree::dentry_tree(wrapped_pointer<mount_point> wp)
	: m_root (std::make_unique<dentry> (nullptr, wp, wp->root ())),
	  m_current_wd("/", path::PATH_UNIX) {
		m_root->is_mount = true;
	}

	dentry_tree::~dentry_tree() = default;

	std::tuple<dentry*, std::shared_ptr<inode>, int, path> dentry_tree::resolve (const path& pth) const {
		path target = m_current_wd;
		target.resolve (pth);
		return _resolve (target, 0, target.depth());
	}

	// unlink all dentries, and returns true iff this dentry is a mount point
	bool dentry_tree::unlink (dentry* victim) {
		bool is_mount = victim->is_mount;
		dentry* parent = victim->parent;
		if (!parent) {
			delete victim;
		} else {
			for (auto itr = parent->children.begin (); itr != parent->children.end (); itr++) {
				if (itr->second == victim) {
					delete itr->second;
					parent->children.erase (itr);
					break;
				}
			}
		}
		return is_mount;
	}

	bool dentry_tree::has_children (dentry* q) {
		if (!q->children.empty ()) {
			return true;
		}
		if (q->ino.expired ()) {
			return false;
		}
		core::stats st;
		auto inoptr = q->ino.lock ();
		try {
			inoptr->stat (st);
		}
		catch (...) {
			return false;
		}
		if (st.type == VFS_INODE_DIRECTORY) {

			auto di = inoptr->get_directory_iterator ();
			if (!di) {
				return false;
			}
			return di->has_next ();
		}
		return false;
	}

	void dentry_tree::mount (wrapped_pointer<mount_point> wp, dentry* node) {
		node->release ();
		node->mount = wp;
		node->ino = wp->root ();
		node->is_mount = true;
	}

	void dentry_tree::cwd(const std::string& wd) {
		auto some_path = m_current_wd;
		some_path.resolve (wd);
		some_path.make_directory();
		auto [dent, ino, depth, p] = _resolve (some_path, 0, some_path.depth());
		if (depth == p.depth ()) {
			core::stats st;
			ino->stat (st);
			if (st.type != VFS_INODE_DIRECTORY) {
				RAISE_EX("Can not change directory to ", p, "which points to file");
			} else {
				m_current_wd = p;
			}
		} else {
			RAISE_EX("Can not change directory to ", p, "since it is missing");
		}
	}

	std::string dentry_tree::cwd() const {
		return m_current_wd.to_string();
	}

	std::tuple<dentry*, std::shared_ptr<inode>, int, path> dentry_tree::_resolve (const path& pth, int from, int to) const {
		wrapped_pointer<mount_point> current_mount = m_root->mount;
		wrapped_pointer<dentry> node (m_root.get());

		int i = 0;
		for (i = from; i < to; i++) {
			const auto& name = pth[i];
			auto itr = node->children.find (name);

			if (itr != node->children.end ()) {
				node = wrapped_pointer<dentry> (itr->second);
				if (node->mount) {
					current_mount = node->mount;
				}
			} else {

				std::shared_ptr<inode> the_inode = node->ino.lock ();
				auto new_inode = the_inode->lookup (name);
				if (new_inode) {

					auto* new_dentry = new dentry (
						node.get (),
						current_mount,
						current_mount->add (std::move (new_inode)));

					node->children.insert (std::make_pair (name, new_dentry));
					node = wrapped_pointer<dentry> (new_dentry);
					if (node->ino.expired ()) {
						break;
					}
				} else {
					break;
				}
			}
		}
		if (node->ino.expired ()) {
			return std::make_tuple (node.get (), nullptr, i, pth);
		}
		return std::make_tuple (node.get (), node->ino.lock (), i, pth);
	}
} // ns vfs::core