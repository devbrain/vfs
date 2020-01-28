#include "api/detail/dentry.hh"
#include <vfs/api/exception.hh>
#include <vfs/api/path.hh>
#include "api/mount_point.hh"


namespace vfs::core {
	struct dentry
	{
		dentry(dentry* dparent, wrapped_pointer<mount_point> mp, std::weak_ptr<inode> pino)
		: parent(dparent),
		mount (mp),
		ino(pino)
		{

		}

		void release ()
		{
			if (mount)
			{
				mount->remove(ino.lock().get());
			}
			for (const auto& kv : children)
			{
				delete kv.second;
			}
		}

		~dentry()
		{
			release();
		}
		dentry* parent;
		wrapped_pointer<mount_point> mount;
		std::weak_ptr<inode> ino;

		std::map<std::string, dentry*> children;
	};

	static dentry* root = nullptr;
	// ======================================================================
	std::tuple<dentry*, std::shared_ptr<inode>, int> dentry_resolve(const path& pth, int from, int to)
	{
		if (!root)
		{
			throw vfs::exception("dentry is not initialized");
		}

		wrapped_pointer<mount_point> current_mount = root->mount;
		wrapped_pointer<dentry> node(root);

		int i = 0;
		for (i=from; i<to; i++)
		{
			const auto name = pth[i];
			auto itr = node->children.find(name);


			if (itr != node->children.end())
			{
				node = wrapped_pointer<dentry>(itr->second);
				if (node->mount)
				{
					current_mount = node->mount;
				}
			}
			else
			{

				std::shared_ptr<inode> the_inode = node->ino.lock();
				auto new_inode = the_inode->lookup(name);
				if (new_inode)
				{

					dentry* new_dentry = new dentry(
						node.get(),
						current_mount,
						current_mount->add(std::move(new_inode)));

					node->children.insert(std::make_pair(name, new_dentry));
					node = wrapped_pointer<dentry>(new_dentry);
					if (node->ino.expired())
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		return std::make_tuple(node.get(), node->ino.lock(), i);
	}
	// ======================================================================
	void dentry_unlink(dentry* victim)
	{
		dentry* parent = victim->parent;
		if (!parent)
		{
			delete victim;
		}
		else
		{
			for (auto itr = parent->children.begin(); itr != parent->children.end(); itr++)
			{
				if (itr->second == victim)
				{
					delete itr->second;
					parent->children.erase(itr);
					break;
				}
			}
		}
	}
	// ======================================================================
	bool dentry_has_children (dentry* victim)
	{
		if (!victim->children.empty())
		{
			return true;
		}
		if (victim->ino.expired())
		{
			return false;
		}
		core::stats st;
		auto inoptr = victim->ino.lock();
		try
		{
			inoptr->stat(st);
		}
		catch (...)
		{
			return false;
		}
		if (st.type == VFS_INODE_DIRECTORY)
		{

			auto di = inoptr->get_directory_iterator();
			if (!di)
			{
				return false;
			}
			return di->has_next();
		}
		return false;
	}
	// ======================================================================
	void dentry_init (wrapped_pointer<mount_point> wp)
	{
		if (root != nullptr)
		{
			throw vfs::exception("dentry is already initialized");
		}
		root = new dentry(nullptr, wp, wp->root());
	}
	// ======================================================================
	void dentry_mount(wrapped_pointer<mount_point> wp, dentry* node)
	{
		node->release();
		node->mount = wp;
		node->ino = wp->root();
	}
	// ======================================================================
	void dentry_done ()
	{
		delete root;
		root = nullptr;
	}
} // ns vfs::core