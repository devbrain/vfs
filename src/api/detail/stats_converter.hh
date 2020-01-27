#ifndef VFS_DETAIL_STATS_CONVERTER_HH
#define VFS_DETAIL_STATS_CONVERTER_HH

#include <vfs/api/stats.hh>
#include "api/filesystem.hh"

namespace vfs
{
	stats convert(const core::stats& st);
}

#endif
