//
// Created by igor on 2/1/24.
//
#include <vfs/io.hh>
#include "system_iface.hh"
#include "detail/io_iface.hh"

namespace vfs {

	std::optional<stats> get_stats (const std::string& pth) {
		return core::get_stats (pth, get_system_dentry_tree());
	}

	// ---------------------------------------------------------------------------------
	directory open_directory (const std::string& pth) {
		return core::open_directory (pth, get_system_dentry_tree());
	}

	// ---------------------------------------------------------------------------------
	void create_directory (const std::string& pth) {
		core::create_directory (pth, get_system_dentry_tree());
	}
	// --------------------------------------------------------------------------------
	void unlink (const std::string& pth) {
		core::unlink (pth, get_system_dentry_tree(), get_system_fstab());
	}
	// ------------------------------------------------------------------------------------------
	file* open (const std::string& pth, unsigned openmode) {
		return core::open (pth, openmode, get_system_dentry_tree());
	}
	// ------------------------------------------------------------------------------------------
	file* open (const std::string& pth, creation_disposition cd, bool readonly) {
		return core::open (pth, cd, readonly, get_system_dentry_tree());
	}

	// ---------------------------------------------------------------------------------------------------------------
	void close (file* f) {
		core::close (f);
	}
	// ---------------------------------------------------------------------------------------------------------------
	size_t read (file* f, void* buff, size_t len) {
		return core::read (f, buff, len);
	}
	// ---------------------------------------------------------------------------------------------------------------
	size_t write (file* f, const void* buff, size_t len) {
		return core::write (f, buff, len);
	}
	// ---------------------------------------------------------------------------------------------------------------
	bool truncate (file* f) {
		return core::truncate (f);
	}

	// ---------------------------------------------------------------------------------------------------------------
	void seek (file* f, uint64_t pos, seek_type whence) {
		return core::seek (f, pos, whence);
	}

	// ---------------------------------------------------------------------------------
	uint64_t tell (file* f) {
		return core::tell (f);
	}
}