//
// Created by igor on 3/18/24.
//


#include <bsw/exception.hh>
#include "tarfs_inode.hh"
#include "tarfs_file.hh"

namespace vfs::extra {

	class directory_iterator : public vfs::module::directory_iterator {
	 public:
		using children_itr_t = tar_tree::children_t::const_iterator;
		directory_iterator(children_itr_t itr, children_itr_t end)
			: _itr(itr),
			  _end(end) {}

		bool has_next () override {
			return _itr != _end;
		}

		std::string next () override {
			std::string ret = _itr->first;
			_itr++;
			return ret;
		}
	 private:
		children_itr_t _itr;
		children_itr_t _end;
	};

	tarfs_inode::tarfs_inode (tarfile* archive, tar_tree* node)
	: vfs::module::inode (node->is_dir ? VFS_INODE_DIRECTORY : VFS_INODE_REGULAR),
	  m_archive(archive),
	  m_node (node) {
	}

	module::inode* tarfs_inode::lookup (const char* name) {
		auto itr = m_node->children.find (name);
		if (itr == m_node->children.end()) {
			return nullptr;
		}
		return new tarfs_inode(m_archive, itr->second);
	}

	uint64_t tarfs_inode::size () {
		return m_node->size;
	}

	vfs::module::directory_iterator* tarfs_inode::get_directory_iterator () {
		return new directory_iterator(m_node->children.begin(), m_node->children.end());;
	}

	bool tarfs_inode::mkdir (const char* name) {
		return false;
	}

	bool tarfs_inode::mkfile (const char* name) {
		return false;
	}

	int tarfs_inode::unlink () {
		return 0;
	}

	bool tarfs_inode::is_sequential () const {
		return false;
	}

	vfs::module::file* tarfs_inode::open_file (open_mode_type mode_type) {
		ENFORCE(mode_type != eVFS_OPEN_MODE_WRITE);
		return new tarfs_file(m_archive, m_node);
	}
}