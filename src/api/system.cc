//
// Created by igor on 26/12/2019.
//

#include <vfs/api/system.hh>
#include <vfs/api/exception.hh>

#include "api/detail/modules_table.hh"
#include "api/detail/fstab.hh"
#include "api/detail/dentry.hh"
#include "api/detail/stats_converter.hh"

#include <bsw/object_manager.hh>
#include <bsw/errors.hh>

namespace vfs
{
	namespace core
	{

		static bool is_root(const std::string& p)
		{
			return p == "/";
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

		fstab = nullptr;
		delete system;

		system = nullptr;
	}
	// ----------------------------------------------------------------------------------
	void load_module(const stdfs::path& path_to_module)
	{
		if (system == nullptr)
		{
			system = new core::system(path_to_module);
            ::core::object_manager::instance().call_on_exit(system_destructor);
		}
		else
		{
			system->add_module(path_to_module);
		}
	}
	// -------------------------------------------------------------------------------------
	void deinitialize()
	{
		system_destructor();
	}
	// -------------------------------------------------------------------------------------
	void mount(const std::string& fstype, const std::string& args, const std::string& mount_point)
	{
		auto* fs = system->get_module(fstype);
		if (!fs)
		{
		    THROW_EXCEPTION_EX(vfs::exception, "Can not find module ", fstype);
		}
		if (fstab == nullptr)
		{
			fstab = new core::fstab;
		}

		if (core::is_root (mount_point))
		{
			auto mountedfs = fstab->mount(fs, path(mount_point), args);
			core::dentry_init(mountedfs);
		}
		else
		{
			path p(mount_point);
			p.make_directory();

			auto[dent, ino, depth] = core::dentry_resolve(p, 0, p.depth());

			if (depth != p.depth())
			{
                THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", mount_point);
			}
			auto mountedfs = fstab->mount(fs, path(mount_point), args);
			core::dentry_mount(mountedfs, dent);
		}
	}
	// -------------------------------------------------------------------------------------
	void unmount (const std::string& mount_point)
	{
		if (!fstab)
		{
            THROW_EXCEPTION_EX(vfs::exception, "no mounted filesystems found");
		}
		fstab->unmount(path(mount_point));
	}
    // -------------------------------------------------------------------------------------
	modules get_modules()
    {
        if (!system)
        {
            THROW_EXCEPTION_EX(vfs::exception, "no modules loaded");
        }
        return modules(&system->_all_modules);
    }
    // -------------------------------------------------------------------------------------
    mounts get_mounts()
    {
        if (!fstab)
        {
            THROW_EXCEPTION_EX(vfs::exception, "no mounted filesystems found");
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
		try
		{
			ino->stat(st);
		} catch (...)
		{
			return std::nullopt;
		}

		auto res = convert(st);

		return std::make_optional(res);
	}
	// ---------------------------------------------------------------------------------
	directory open_directory (const std::string& pth)
	{
		path p (pth);
		p.make_directory();

		auto [dent, ino, depth] = core::dentry_resolve(p, 0, p.depth());
		if (depth != p.depth())
		{
            THROW_EXCEPTION_EX(vfs::exception, "path not found ", pth);
		}
		core::stats st;
		ino->stat(st);
		if (st.type != VFS_INODE_DIRECTORY)
		{
            THROW_EXCEPTION_EX(vfs::exception, "path ", pth, " is not directory");
		}
		return directory(ino);
	}
	// ---------------------------------------------------------------------------------
	void create_directory (const std::string& pth)
	{
		path p (pth);
		p.make_directory();

		auto [dent, ino, depth] = core::dentry_resolve(p, 0, p.depth());
		if (depth == p.depth())
		{
            THROW_EXCEPTION_EX(vfs::exception, "path ", pth, " already exists");
		}
		if (depth != p.depth() - 1)
		{
            THROW_EXCEPTION_EX(vfs::exception, "path ", pth, "has to many non existing components");
		}
		p.make_file();
		if (!ino->mkdir(p.get_file_name()))
		{
            THROW_EXCEPTION_EX(vfs::exception, "failed to create directory ", pth);
		}
	}
	// --------------------------------------------------------------------------------
	void unlink (const std::string& pth)
	{
		path p (pth);
		p.make_directory();

		auto [dent, ino, depth] = core::dentry_resolve(p, 0, p.depth());
		if (depth != p.depth())
		{
            THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
		}
		if (dentry_has_children (dent))
		{
            THROW_EXCEPTION_EX(vfs::exception, "path ", pth, " is non empty directory");

		}
		if (!ino->unlink())
		{
            THROW_EXCEPTION_EX(vfs::exception, "failed to unlink ", pth);
		}
		if (dentry_unlink(dent))
		{
			fstab->unmount(p);
		}
	}
} // ns vfs
