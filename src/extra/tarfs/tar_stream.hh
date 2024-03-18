//
// Created by igor on 3/18/24.
//

#ifndef VFS_SRC_EXTRA_TARFS_TAR_STREAM_HH_
#define VFS_SRC_EXTRA_TARFS_TAR_STREAM_HH_

#include "microtar.h"
#include <iosfwd>
#include <memory>

namespace vfs::extra {
	std::unique_ptr<mtar_t> create_mtar_from_stream (std::istream* is);
	std::unique_ptr<mtar_t> create_mtar_from_stream (std::ostream* os);
}
#endif //VFS_SRC_EXTRA_TARFS_TAR_STREAM_HH_
