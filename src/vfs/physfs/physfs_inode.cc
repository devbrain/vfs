//
// Created by igor on 02/01/2020.
//

#include <cstdio>
#include <bsw/predef.h>
#include <bsw/macros.hh>

#if !defined(PREDEF_PLATFORM_WIN32)
#include <unistd.h>
#define SLIT(X) X
#else
#include <io.h>
#define fopen _wfopen
#define fseek _fseeki64
#define SLIT(X) PPCAT(L, X)
#endif

#include <utility>
#include "physfs_inode.hh"

namespace vfs::detail {

	class file : public vfs::module::file {
	 public:
		explicit file (FILE* f);
	 private:
		~file () override;
		ssize_t read (void* buff, size_t len) override;
		ssize_t write (void* buff, size_t len) override;
		bool seek (uint64_t pos, enum whence_type whence) override;
		bool truncate () override;
		[[nodiscard]] uint64_t tell () const override;
	 private:
		FILE* _file;
	};

	class directory_iterator : public vfs::module::directory_iterator {
	 public:
		explicit directory_iterator (std::filesystem::directory_iterator di)
			: _di (std::move(di)),
			  _itr (begin (_di)),
			  _end (end (_di)) {

		}

		bool has_next () override {
			return _itr != _end;
		}

		std::string next () override {
			std::string ret = _itr->path ().filename ().string ();
			_itr++;
			return ret;
		}

	 private:
		std::filesystem::directory_iterator _di;
		std::filesystem::directory_iterator _itr;
		std::filesystem::directory_iterator _end;
	};

// ===================================================================================
	physfs_inode::physfs_inode (const std::filesystem::path& pth)
		: vfs::module::inode (std::filesystem::is_directory (pth) ? VFS_INODE_DIRECTORY : VFS_INODE_REGULAR),
		  _path (pth) {

	}

// -----------------------------------------------------------------------------------
	vfs::module::inode* physfs_inode::lookup (const char* name) {
		const std::filesystem::path child = _path / name;

		if (!std::filesystem::exists (child)) {
			return nullptr;
		}
		return new physfs_inode (child);
	}

// -----------------------------------------------------------------------------------
	vfs::module::directory_iterator* physfs_inode::get_directory_iterator () {
		return new directory_iterator (std::filesystem::directory_iterator (_path));
	}

// -----------------------------------------------------------------------------------
	uint64_t physfs_inode::size () {
		if (VFS_INODE_DIRECTORY == type ()) {
			return 0;
		}
		return std::filesystem::file_size (_path);
	}

// -----------------------------------------------------------------------------------
	bool physfs_inode::mkdir (const char* name) {
		const std::filesystem::path child = _path / name;
		if (std::filesystem::exists (child)) {
			return false;
		}
		return std::filesystem::create_directory (child);
	}

// -----------------------------------------------------------------------------------
	bool physfs_inode::mkfile (const char* name) {
		const std::filesystem::path child = _path / name;
		if (std::filesystem::exists (child)) {
			return false;
		}
		FILE* f = fopen (child.c_str (), SLIT("wb"));
		if (!f) {
			return false;
		}
		fclose (f);
		return true;
	}

// -----------------------------------------------------------------------------------
	int physfs_inode::unlink () {
		if (!std::filesystem::exists (_path)) {
			return 0;
		}
		return std::filesystem::remove (_path) ? 1 : 0;
	}

// -----------------------------------------------------------------------------------
	vfs::module::file* physfs_inode::open_file (open_mode_type mode_type) {
		FILE* f = nullptr;

		if (mode_type == eVFS_OPEN_MODE_READ) {
			f = fopen (_path.c_str (), SLIT("rb"));
		} else {
			f = fopen (_path.c_str (), SLIT("rb+"));
		}
		if (!f) {
			return nullptr;
		}
		return new file (f);
	}

// ===========================================================================================
	file::file (FILE* f)
		: _file (f) {

	}

// -----------------------------------------------------------------------------------
	file::~file () {
		fclose (_file);
	}

// -----------------------------------------------------------------------------------
	ssize_t file::read (void* buff, size_t len) {

		return fread (buff, 1, len, _file);

	}

// -----------------------------------------------------------------------------------
	ssize_t file::write (void* buff, size_t len) {
		auto rc = fwrite (buff, 1, len, _file);
		return rc;
	}

// -----------------------------------------------------------------------------------
	bool file::seek (uint64_t pos, enum whence_type whence) {
		int w = SEEK_SET;
		switch (whence) {
			case eVFS_SEEK_SET:w = SEEK_SET;
				break;
			case eVFS_SEEK_END:w = SEEK_END;
				break;
			case eVFS_SEEK_CUR:w = SEEK_CUR;
				break;
		}
		return fseek (_file, pos, w) == 0;
	}

// -----------------------------------------------------------------------------------
	bool file::truncate () {
#if !defined(PREDEF_PLATFORM_WIN32)
		return ftruncate (fileno (_file), 0) == 0;
#else
		return _chsize(fileno(_file), 0) == 0;
#endif
	}

// -----------------------------------------------------------------------------------
	uint64_t file::tell () const {
		return ftell (_file);
	}
}