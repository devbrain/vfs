//
// Created by igor on 22/12/2019.
//

#ifndef VFS_HASHER_HH
#define VFS_HASHER_HH

#include <tuple>
#include <cstdint>
#include <string>
#include <vector>

#include "vfs/api/vfs_api.h"


namespace vfs
{
    namespace core
    {
        using hash_t = std::tuple<std::uint64_t, std::uint64_t>;

        VFS_API hash_t hash(const char* data, std::size_t sz);
        VFS_API hash_t hash(const char* data, std::size_t sz, const hash_t& seed);

        VFS_API hash_t hash(const std::string& data);
        VFS_API hash_t hash(const std::string& data, const hash_t& seed);

    } // ns core
} // ns vfs

#endif //VFS_HASHER_HH
