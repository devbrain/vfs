//
// Created by igor on 3/5/24.
//

#include <fstream>
#include "vfs/extra/floppyfs.hh"
#include "floppyfs_impl.hh"
#include "floppyfs_inode.hh"

namespace vfs::extra {

	floppyfs::floppyfs ()
	: vfs::module::filesystem ("floppyfs"),
	  m_istream(nullptr),
	  m_fat(nullptr) {
	}

	vfs::module::inode* floppyfs::load_root (const std::string& params) {
		m_istream = std::make_unique<std::fstream>(params, std::ios::binary | std::ios::in | std::ios::out);
		if (!m_istream->good()) {
			return nullptr;
		}
		std::istream* is = m_istream.get ();
		m_fat = std::make_unique<fat12>(*is);
		return new floppyfs_inode(m_fat.get(), 0, true, 0);
	}

	size_t floppyfs::max_name_length () {
		return 11;
	}

	int floppyfs::sync () {
		m_istream->sync();
		m_istream->flush();
		return 1;
	}

	int floppyfs::sync_inode (vfs::module::inode* inod) {
		return 0;
	}

	bool floppyfs::is_readonly () const {
		return false;
	}

	floppyfs::~floppyfs () = default;

	std::unique_ptr<vfs::module::filesystem> create_floppyfs() {
		return std::make_unique<floppyfs>();
	}
}

#if !defined(FLOPPYFS_STATIC_DEFINE)
REGISTER_VFS_MODULE(vfs::extra::floppyfs)
#endif

