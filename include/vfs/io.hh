//
// Created by igor on 1/31/24.
//

#ifndef VFS_INCLUDE_VFS_IO_HH_
#define VFS_INCLUDE_VFS_IO_HH_

#include <string>
#include <optional>

#include <vfs/api/vfs_api.h>
#include <vfs/stats.hh>
#include <vfs/directory_iterator.hh>

#include <bsw/warn/push.hh>
#include <bsw/warn/dll_interface>

namespace vfs {

	VFS_API std::optional<stats> get_stats (const std::string& pth);
	VFS_API directory open_directory (const std::string& pth);
	VFS_API void create_directory (const std::string& pth);
	VFS_API void unlink (const std::string& pth);

	enum class creation_disposition {
		/*
		 * Creates a new file, always.
		 * If the specified file exists and is writable, the function overwrites the file, the function succeeds, and last-error code is set to ERROR_ALREADY_EXISTS (183).
		 * If the specified file does not exist and is a valid path, a new file is created, the function succeeds, and the last-error code is set to zero.
		*/
		eCREATE_ALWAYS,
		/*
		 * Creates a new file, only if it does not already exist.
		 * If the specified file exists, the function fails and the last-error code is set to ERROR_FILE_EXISTS (80).
		 * If the specified file does not exist and is a valid path to a writable location, a new file is created.
		 */
		eCREATE_NEW,
		/*
		 * Opens a file or device, only if it exists.
		 * If the specified file or device does not exist, the function fails and the last-error code is set to ERROR_FILE_NOT_FOUND (2).
		 */
		eOPEN_EXISTING,
		/* Opens a file and truncates it so that its size is zero bytes, only if it exists.
		 * If the specified file does not exist, the function fails and the last-error code is set to ERROR_FILE_NOT_FOUND (2).
		 */
		eTRUNCATE_EXISTING
	};

	struct file;

	VFS_API file* open (const std::string& path, creation_disposition cd, bool readonly);

	enum openmode_t : unsigned {
		TRUNCATE   = 0x1,
		APPEND     = 0x2,
		CREATE     = 0x4,
		READ_ONLY  = 0x8,
		WRITE_ONLY = 0x10,
		READ_WRITE = READ_ONLY | WRITE_ONLY
	};

	VFS_API file* open (const std::string& path, unsigned openmode);

	VFS_API void close (file* f);

	VFS_API size_t read (file* f, void* buff, size_t len);
	VFS_API size_t write (file* f, const void* buff, size_t len);
	VFS_API bool truncate (file* f);

	enum class seek_type {
		eCUR,
		eSET,
		eEND
	};

	VFS_API void seek (file* f, uint64_t pos, seek_type whence);
	VFS_API uint64_t tell (file* f);

}
#include <bsw/warn/pop.hh>

#endif //VFS_INCLUDE_VFS_IO_HH_
