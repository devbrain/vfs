//
// Created by igor on 26/12/2019.
//

#include "vfs/system.hh"
#include <utility>

#include "vfs/exception.hh"

#include "detail/modules_table.hh"
#include "detail/fstab.hh"
#include "detail/dentry.hh"
#include "system_iface.hh"

#include <bsw/register_at_exit.hh>
#include <bsw/errors.hh>

namespace vfs {
	namespace core {

		static bool is_root (const std::string& p) {
			return p == "/";
		}

		struct system {
			system() = default;

			explicit system (const std::filesystem::path& modules_path);
			explicit system(std::unique_ptr<module::filesystem> fsptr);

			void add_module (const std::filesystem::path& modules_path, modules_loading_report* report);
			void add_module (std::unique_ptr<module::filesystem> fsptr);

			[[nodiscard]] file_system* get_module (const std::string& type) const;

			~system ();

			modules_table _all_modules;
			std::unique_ptr<core::fstab> fstab;
			std::unique_ptr<core::dentry_tree> dentry;
		};

		// =============================================================================
		system::system (const std::filesystem::path& modules_path)
			: _all_modules (modules_path) {
		}

		system::system(std::unique_ptr<module::filesystem> fsptr) : _all_modules (std::move(fsptr), true) {
		}
		// ------------------------------------------------------------------------------
		void system::add_module (std::unique_ptr<module::filesystem> fsptr) {
			_all_modules.add (std::move (fsptr));
		}
		// ------------------------------------------------------------------------------
		void system::add_module (const std::filesystem::path& modules_path, modules_loading_report* report) {
			_all_modules.add (modules_path, report);
		}

		// ------------------------------------------------------------------------------
		file_system* system::get_module (const std::string& type) const {
			return _all_modules.get (type);
		}

		// ------------------------------------------------------------------------------
		system::~system () = default;
	} // ns detail
	// ==================================================================================
	static core::system* system = nullptr;

	core::fstab* get_system_fstab() {
		ENFORCE(system);
		ENFORCE(system->fstab);
		return system->fstab.get();
	}

	core::dentry_tree* get_system_dentry_tree() {
		ENFORCE(system);
		ENFORCE(system->dentry);
		return system->dentry.get();
	}

//	void fstab_unmount(const path& p) {
//		system->fstab->unmount (p);
//	}

	static void system_destructor () {
		if (system != nullptr) {
			delete system;
			system = nullptr;
		}
	}

	// ----------------------------------------------------------------------------------
	void load_module (const std::filesystem::path& path_to_module, modules_loading_report* report) {
		if (system == nullptr) {
			system = new core::system (path_to_module);
			bsw::register_at_exit (system_destructor);
		} else {
			system->add_module (path_to_module, report);
		}
	}
	// -------------------------------------------------------------------------------------
	void load_module (std::unique_ptr<module::filesystem> fsptr) {
		if (system == nullptr) {
			system = new core::system (std::move(fsptr));
			bsw::register_at_exit (system_destructor);
		} else {
			system->add_module (std::move(fsptr));
		}
	}
	// -------------------------------------------------------------------------------------
	void deinitialize () {
		system_destructor ();
	}

	// -------------------------------------------------------------------------------------
	void mount (const std::string& fstype, const std::string& args, const std::string& mount_point) {
		if (system == nullptr) {
			system = new core::system;
			bsw::register_at_exit (system_destructor);
		}
		auto* fs = system->get_module (fstype);
		if (!fs) {
			THROW_EXCEPTION_EX(vfs::exception, "Can not find module ", fstype);
		}
		if (!system->fstab) {
			system->fstab = std::make_unique<core::fstab>();
		}


		path p (mount_point);
		p.make_directory ();
		if (!p.is_absolute()) {
			THROW_EXCEPTION_EX(vfs::exception, "Mount point ", mount_point, " should be absolute");
		}
		if (core::is_root (mount_point)) {
			auto mountedfs = system->fstab->mount (fs, path (mount_point), args);
			system->dentry = std::make_unique<core::dentry_tree> (mountedfs);
		} else {
			auto [dent, ino, depth, _] = system->dentry->resolve (p);

			if (depth != p.depth ()) {
				THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", mount_point);
			}
			auto mountedfs = system->fstab->mount (fs, p, args);
			core::dentry_tree::mount (mountedfs, dent);
		}
	}

	// -------------------------------------------------------------------------------------
	void unmount (const std::string& mount_point) {
		if (!system->fstab) {
			THROW_EXCEPTION_EX(vfs::exception, "no mounted filesystems found");
		}
		system->fstab->unmount (path (mount_point));
	}

	// -------------------------------------------------------------------------------------
	modules get_modules () {
		if (!system) {
			system = new core::system;
			bsw::register_at_exit (system_destructor);
		}
		return modules (&system->_all_modules);
	}

	// -------------------------------------------------------------------------------------
	mounts get_mounts () {
		if (!system->fstab) {
			THROW_EXCEPTION_EX(vfs::exception, "no mounted filesystems found");
		}
		return mounts (system->fstab.get());
	}

	// -------------------------------------------------------------------------------------
	void set_cwd(const std::string& wd) {
		ENFORCE(system->dentry);
		system->dentry->cwd (wd);
	}
	// -------------------------------------------------------------------------------------
	std::string get_cwd() {
		ENFORCE(system->dentry);
		return system->dentry->cwd();
	}
} // ns vfs
