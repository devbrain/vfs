//
// Created by igor on 3/19/24.
//

#ifndef VFS_SRC_EXTRA_LHAFS_LHA_ARCHIVE_HH_
#define VFS_SRC_EXTRA_LHAFS_LHA_ARCHIVE_HH_

#include <iosfwd>
#include "lhasa/public/lhasa.h"

namespace vfs::extra {
	class lha_archive {
	 public:
		explicit lha_archive(std::istream& is);
		~lha_archive();
	 private:
		std::istream&      m_stream;
		LHAInputStreamType m_rdr_ops;
		LHAInputStream*    m_lha_stream;
		LHAReader*         m_reader;
	};
}

#endif //VFS_SRC_EXTRA_LHAFS_LHA_ARCHIVE_HH_
