//
// Created by igor on 3/18/24.
//


#include <vfs/extra/tarfs.hh>
#include <fstream>

#include "tarfs_inode.hh"
#include "tarfs_impl.hh"

namespace vfs::extra {

	tarfs_impl::tarfs_impl ()
		: vfs::module::filesystem ("tarfs") {
	}

	vfs::module::inode* tarfs_impl::load_root (const std::string& params) {
		m_istream = std::make_unique<std::ifstream> (params, std::ios::binary | std::ios::in);
		if (!m_istream->good()) {
			return nullptr;
		}
		m_tar = std::make_unique<tar_archive>(*m_istream);
		return new tarfs_inode(m_tar.get(), m_tar->get_root());
	}

	int tarfs_impl::sync () {
		return 1;
	}

	size_t tarfs_impl::max_name_length () {
		return 100;
	}

	int tarfs_impl::sync_inode (vfs::module::inode* inod) {
		return 1;
	}

	bool tarfs_impl::is_readonly () const {
		return true;
	}

	tarfs_impl::~tarfs_impl () = default;

	std::unique_ptr<vfs::module::filesystem> create_tarfs() {
		return std::make_unique<tarfs_impl>();
	}

}

#if !defined(TARFS_STATIC_DEFINE)
REGISTER_VFS_MODULE(vfs::extra::tarfs_impl)
#endif