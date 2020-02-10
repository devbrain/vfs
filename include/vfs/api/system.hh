//
// Created by igor on 26/12/2019.
//

#ifndef SYSTEM_HH
#define SYSTEM_HH

#include <map>
#include <optional>

#include <vfs/api/stdfilesystem.hh>
#include <vfs/api/vfs_api.h>
#include <api/detail/path.hh>
#include <vfs/api/modules.hh>
#include <vfs/api/mounts.hh>
#include <vfs/api/stats.hh>
#include <vfs/api/directory_iterator.hh>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning( disable : 4251 )
#endif

namespace vfs
{
    VFS_API void deinitialize();

    VFS_API void load_module(const stdfs::path& path_to_module);
    VFS_API modules get_modules();

    VFS_API void mount(const std::string& fstype, const std::string& args, const std::string& mount_point);
    VFS_API mounts get_mounts();
    VFS_API void unmount(const std::string& mount_point);

    VFS_API std::optional<stats> get_stats(const std::string& pth);
    VFS_API  directory open_directory(const std::string& pth);
    VFS_API void create_directory(const std::string& pth);
    VFS_API void unlink(const std::string& pth);

    enum class creation_disposition
    {
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
         * Opens a file, always.
         * If the specified file exists, the function succeeds and the last-error code is set to ERROR_ALREADY_EXISTS (183).
         * If the specified file does not exist and is a valid path to a writable location, the function creates a file and the last-error code is set to zero.
         */
                eOPEN_ALWAYS,
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

    class file;

    VFS_API file* open(const std::string& path, creation_disposition cd, bool readonly);
    void close(file* f);

    VFS_API size_t read(file* f, void* buff, size_t len);
    VFS_API size_t write(file* f, const void* buff, size_t len);
    VFS_API bool truncate(file* f);

    enum class seek_type
    {
        eSTART,
        eSET,
        eEND
    };

    VFS_API uint64_t seek(file* f, uint64_t pos, seek_type whence);

} // ns vfs

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif //SYSTEM_HH
