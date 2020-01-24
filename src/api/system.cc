//
// Created by igor on 26/12/2019.
//

#include <cstdlib>

#include "vfs/api/system.hh"
#include "vfs/api/exception.hh"

#include "api/detail/modules_table.hh"
#include "api/detail/fstab.hh"
#include "api/detail/dentry.hh"

namespace vfs
{
	namespace core
	{

		static bool is_root(const path& p)
		{
			return p.empty();
		}

		struct system
		{
		public:
			explicit system(const stdfs::path& modules_path);
			void add_module(const stdfs::path& modules_path);

			[[nodiscard]] filesystem* get_module(const std::string& type) const;

			~system();

			modules_table _all_modules;
		};
		// =============================================================================
		system::system(const stdfs::path& modules_path)
			: _all_modules(modules_path)
		{
		}
		// ------------------------------------------------------------------------------
		void system::add_module(const stdfs::path& modules_path)
		{
			_all_modules.add(modules_path);
		}
		// ------------------------------------------------------------------------------
		filesystem* system::get_module(const std::string& type) const
		{
			return _all_modules.get(type);
		}
		// ------------------------------------------------------------------------------
		system::~system() = default;
	} // ns detail
	// ==================================================================================
	static core::system* system = nullptr;
	static core::fstab* fstab = nullptr;

	static void system_destructor()
	{
		core::dentry_done();
		delete fstab;
		delete system;
	}
	// ----------------------------------------------------------------------------------
	void load_module(const stdfs::path& path_to_module)
	{
		if (system == nullptr)
		{
			system = new core::system(path_to_module);
			std::atexit(system_destructor);
		}
		else
		{
			system->add_module(path_to_module);
		}
	}
	// -------------------------------------------------------------------------------------
	void mount(const std::string& fstype, const std::string& args, const path& mount_point)
	{
		auto* fs = system->get_module(fstype);
		if (!fs)
		{
			throw vfs::exception("Can not find module");
		}
		if (fstab == nullptr)
		{
			fstab = new core::fstab;
		}
		auto mountedfs = fstab->mount(fs, mount_point, args);
		if (core::is_root (mount_point))
		{
			core::dentry_init(mountedfs);
		}
	}
	// -------------------------------------------------------------------------------------
	void unmount (const path& mount_point)
	{
		if (fstab == nullptr)
		{
			throw exception("no mounted filesystems found");
		}
		fstab->unmount(mount_point);
	}
    // -------------------------------------------------------------------------------------
	modules get_modules()
    {
        if (!system)
        {
            throw std::runtime_error("No modules loaded");
        }
        return modules(&system->_all_modules);
    }
    // -------------------------------------------------------------------------------------
    mounts get_mounts()
    {
        if (!fstab)
        {
            throw std::runtime_error("No modules loaded");
        }
        return mounts(fstab);
    }
	// -------------------------------------------------------------------------------------
    std::optional<stats> get_stats(const std::string& pth)
	{
		path p (pth);
		p.make_directory();

		auto [dent, ino, depth] = core::dentry_resolve(p, 0, p.depth());

		if (depth != p.depth())
		{
			return std::nullopt;
		}
		core::stats st;
		ino->stat(st);

		stats res;
		res.size = st.size;
		switch (st.type)
		{
		case VFS_INODE_DIRECTORY:
			res.type = stats::eDIRECTORY;
			break;
		case VFS_INODE_REGULAR:
			res.type = stats::eFILE;
			break;
		default:
			res.type = stats::eLINK;
		}
		if (st.attr1.key[0] != 0)
		{
			res.attribs.insert(std::make_pair(st.attr1.key, st.attr1.value));
		}
		if (st.attr2.key[0] != 0)
		{
			res.attribs.insert(std::make_pair(st.attr2.key, st.attr2.value));
		}
		for (std::size_t i=0; i<st.num_of_additional_attributes; i++)
		{
			res.attribs.insert(std::make_pair(st.additional_attributes[i].key,st.additional_attributes[i].value));
		}
		return std::make_optional(res);
	}
} // ns vfs
