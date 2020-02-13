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
			auto mountedfs = fstab->mount(fs, p, args);
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
		    p.make_directory();
			fstab->unmount(p);
		}
	}
    // ------------------------------------------------------------------------------------------
    struct file
    {
        file(std::unique_ptr<core::file_ops>&& fops, const std::string& pth)
        : file_ops(std::move(fops)),
          file_path (pth)
        {

        }
        std::unique_ptr<core::file_ops> file_ops;
        std::string file_path;
	};
    // ------------------------------------------------------------------------------------------
	file* open(const std::string& pth, creation_disposition cd, bool readonly)
    {
        path p (pth);
        p.make_directory();
        if (readonly)
        {
            if (cd == creation_disposition::eCREATE_ALWAYS)
            {
                THROW_EXCEPTION_EX(vfs::exception, "Mismatched arguments CREATE_ALWAYS and readonly");
            }
            if (cd == creation_disposition::eTRUNCATE_EXISTING)
            {
                THROW_EXCEPTION_EX(vfs::exception, "Mismatched arguments TRUNCATE_EXISTING and readonly");
            }

        }
        // TODO check read only fs
        auto [dent, ino, depth] = core::dentry_resolve(p, 0, p.depth());
        std::unique_ptr<core::file_ops> fops;
        if (depth == p.depth())
        {
            switch (cd)
            {
                case creation_disposition::eCREATE_ALWAYS:
                case creation_disposition::eTRUNCATE_EXISTING:
                    fops = ino->get_file_ops(eVFS_OPEN_MODE_WRITE);
                    if (!fops)
                    {
                        THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth);
                    }
                    if (!fops->truncate())
                    {
                        THROW_EXCEPTION_EX(vfs::exception, "failed to truncate file ", pth);
                    }
                    return new file(std::move(fops), pth);
                case creation_disposition::eCREATE_NEW:
                case creation_disposition::eOPEN_EXISTING:
                    fops = ino->get_file_ops(readonly ? eVFS_OPEN_MODE_READ : eVFS_OPEN_MODE_WRITE);
                    if (!fops)
                    {
                        THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth);
                    }
                    return new file(std::move(fops), pth);
            }
        }
        else
        {
            if (depth == p.depth() - 1)
            {
                p.make_file();

                if (readonly)
                {
                    THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " for reading, since it does not exists");
                }
                if (cd == creation_disposition::eOPEN_EXISTING || cd == creation_disposition::eTRUNCATE_EXISTING)
                {
                    THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
                }
                if (!ino->mkfile(p.get_file_name()))
                {
                    THROW_EXCEPTION_EX(vfs::exception, "can not create new file ", pth);
                }
                p.make_directory();
                auto [dent_new, ino_new, depth_new] = core::dentry_resolve(p, 0, p.depth());
                if (depth_new < p.depth())
                {
                    THROW_EXCEPTION_EX(vfs::exception, "can not resolve new file ", pth);
                }
                fops = ino_new->get_file_ops(eVFS_OPEN_MODE_WRITE);
                if (!fops)
                {
                    THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " for writing");
                }
                return new file(std::move(fops), pth);
            } else
            {
                THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
            }
        }
        THROW_EXCEPTION("Should not be here ", pth);
    }
    // ---------------------------------------------------------------------------------------------------------------
    void close(file* f)
    {
	    if (f && f->file_ops)
        {
            delete f;
        }
    }
    // ---------------------------------------------------------------------------------------------------------------
    size_t read(file* f, void* buff, size_t len)
    {
        if (f && f->file_ops)
        {
            auto rc = f->file_ops->read(buff, len);
            if (rc < 0)
            {
                THROW_EXCEPTION_EX(vfs::exception, "read failed from ", f->file_path);
            }
            return static_cast<size_t>(rc);
        }
        THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
    }
    // ---------------------------------------------------------------------------------------------------------------
    size_t write(file* f, const void* buff, size_t len)
    {
        if (f && f->file_ops)
        {
            auto rc = f->file_ops->write(buff, len);
            if (rc < 0)
            {
                THROW_EXCEPTION_EX(vfs::exception, "write failed to ", f->file_path);
            }
            return static_cast<size_t>(rc);
        }
        THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
    }
    // ---------------------------------------------------------------------------------------------------------------
    bool truncate(file* f)
    {
        if (f && f->file_ops)
        {
            return f->file_ops->truncate();
        }
        THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
    }
    // ---------------------------------------------------------------------------------------------------------------
    void seek(file* f, uint64_t pos, seek_type whence)
    {
        if (f && f->file_ops)
        {
            whence_type wt;
            switch (whence)
            {
                case seek_type::eCUR:
                    wt = eVFS_SEEK_CUR;
                    break;
                case seek_type::eEND:
                    wt = eVFS_SEEK_END;
                    break;
                case seek_type::eSET:
                    wt = eVFS_SEEK_SET;
                    break;
            }
            auto rc = f->file_ops->seek(pos, wt);
            if (!rc)
            {
                THROW_EXCEPTION_EX(vfs::exception, "seek to ", pos, " failed in ", f->file_path);
            }

        } else
        {
            THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
        }
    }
    // ---------------------------------------------------------------------------------
    uint64_t tell (file* f)
    {
        if (f && f->file_ops)
        {
            auto rc = f->file_ops->tell();
            if (rc == static_cast<uint64_t>(-1))
            {
                THROW_EXCEPTION_EX(vfs::exception, "tell failed for ", f->file_path);
            }
            return rc;
        }
        THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
    }
} // ns vfs
