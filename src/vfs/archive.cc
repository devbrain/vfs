//
// Created by igor on 2/1/24.
//

#include <vfs/archive.hh>
#include "detail/dentry.hh"
#include "detail/fstab.hh"
#include "detail/path.hh"
#include "detail/modules_table.hh"
#include "detail/io_iface.hh"

namespace vfs {

	struct archive::impl {
		explicit impl(std::unique_ptr<module::filesystem> fsptr, const std::string& args)
		: m_all_modules(std::move(fsptr), false),
		  m_fstab (new core::fstab),
		  m_dentry(nullptr) {
			auto* fs = m_all_modules.get_single();
			auto mountedfs = m_fstab->mount (fs, path ("/", path::PATH_UNIX), args);
			m_dentry = new core::dentry_tree (mountedfs);
		}

		explicit impl(const std::string& args)
			: m_all_modules(nullptr, true),
			  m_fstab (new core::fstab),
			  m_dentry(nullptr) {
			auto* fs = m_all_modules.get_single();
			auto mountedfs = m_fstab->mount (fs, path ("/", path::PATH_UNIX), args);
			m_dentry = new core::dentry_tree (mountedfs);
		}

		~impl() {
			delete m_dentry;
			delete m_fstab;
		}
		core::modules_table m_all_modules;
		core::fstab* m_fstab;
		core::dentry_tree* m_dentry;
	};

	archive::archive (std::unique_ptr<module::filesystem> fsptr, const std::string& args)
	: m_pimpl(spimpl::make_impl<impl>(std::move(fsptr), args)) {
	}

	archive::~archive() = default;

	std::optional<stats> archive::get_stats (const std::string& pth) {
		return core::get_stats (pth, m_pimpl->m_dentry);
	}

	directory archive::open_directory (const std::string& pth) {
		return core::open_directory (pth, m_pimpl->m_dentry);
	}

	void archive::create_directory (const std::string& pth) {
		core::create_directory (pth, m_pimpl->m_dentry);
	}

	void archive::unlink (const std::string& pth) {
		core::unlink (pth, m_pimpl->m_dentry, m_pimpl->m_fstab);
	}

	file* archive::open (const std::string& path, unsigned int openmode) {
		return core::open (path, openmode, m_pimpl->m_dentry);
	}

	void archive::close (file* f) {
		core::close (f);
	}

	size_t archive::read (file* f, void* buff, size_t len) {
		return core::read (f, buff, len);
	}

	size_t archive::write (file* f, const void* buff, size_t len) {
		return core::write (f, buff, len);
	}

	bool archive::truncate (file* f) {
		return core::truncate (f);
	}

	void archive::seek (file* f, uint64_t pos, seek_type whence) {
		return core::seek (f, pos, whence);
	}

	uint64_t archive::tell (file* f) {
		return core::tell (f);
	}
	// ============================================================================
	physfs_archive::physfs_archive(const std::string& args)
	: archive (nullptr, args) {
	}
}