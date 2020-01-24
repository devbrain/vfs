#include "api/detail/dentry.hh"
#include <vfs/api/exception.hh>
#include <vfs/api/path.hh>
#include "api/mount_point.hh"


namespace vfs::core {
	struct dentry
	{
		dentry(dentry* dparent, wrapped_pointer<mount_point> mp, wrapped_pointer<inode> pino)
		: parent(dparent),
		mount (mp),
		ino(pino)
		{

		}

		~dentry()
		{
			for (const auto& kv : children)
			{
				delete kv.second;
			}
		}
		dentry* parent;
		wrapped_pointer<mount_point> mount;
		wrapped_pointer<inode> ino;

		std::map<std::string, dentry*> children;
	};

	static dentry* root = nullptr;
	// ======================================================================
	std::tuple<dentry*, inode*, int> dentry_resolve(const path& pth, int from, int to)
	{
		if (!root)
		{
			throw vfs::exception("dentry is not initialized");
		}

		wrapped_pointer<mount_point> current_mount = root->mount;
		wrapped_pointer<inode> ino = root->ino;
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
				ino = node->ino;
			}
			else
			{
				auto new_inode = ino->lookup(name);
				if (new_inode)
				{

					dentry* new_dentry = new dentry(
						node.get(),
						current_mount,
						current_mount->add(std::move(new_inode)));

					node->children.insert(std::make_pair(name, new_dentry));
					node = wrapped_pointer<dentry>(new_dentry);
				}
				else
				{
					break;
				}
			}
		}
		return std::make_tuple(node.get(), node->ino.get(), i);
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
	void dentry_done ()
	{
		delete root;
	}
} // ns vfs::core