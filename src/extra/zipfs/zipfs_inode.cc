//
// Created by igor on 2/18/24.
//

#include <bsw/exception.hh>
#include "zipfs_uncompressed_file.hh"
#include "zipfs_inmem_file.hh"
#include "zipfs_inode.hh"
#include "zipfs_stream_file.hh"


namespace vfs::extra {

	class directory_iterator : public vfs::module::directory_iterator {
	 public:
		using children_itr_t = zip_tree::children_t::const_iterator;
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

	// ============================================================================

	zipfs_inode::zipfs_inode (zip_archive* archive, zip_tree* node)
	: vfs::module::inode (node->is_dir ? VFS_INODE_DIRECTORY : VFS_INODE_REGULAR),
	  m_archive(archive),
	  m_node (node) {
	}

	module::inode* zipfs_inode::lookup (const char* name) {
		auto itr = m_node->children.find (name);
		if (itr == m_node->children.end()) {
			return nullptr;
		}
		return new zipfs_inode(m_archive, itr->second);
	}

	uint64_t zipfs_inode::size () {
		return m_node->original_size;
	}

	bool zipfs_inode::mkdir ([[maybe_unused]] const char* name) {
		return false;
	}

	bool zipfs_inode::mkfile ([[maybe_unused]] const char* name) {
		return false;
	}

	int zipfs_inode::unlink () {
		return 0;
	}

	vfs::module::directory_iterator* zipfs_inode::get_directory_iterator () {
		return new directory_iterator(m_node->children.begin(), m_node->children.end());
	}

	vfs::module::file* zipfs_inode::open_file (open_mode_type mode_type) {
		ENFORCE(mode_type != eVFS_OPEN_MODE_WRITE);
		if (!m_node->is_compressed) {
			return new zipfs_uncompressed_file(m_archive, m_node);
		}
		if (m_node->original_size < zipfs_stream_file::OUTPUT_BUFFER_SIZE) {
			return new zipfs_inmem_file (m_archive, m_node);
		}
		return new zipfs_stream_file(m_archive, m_node);
	}

	bool zipfs_inode::is_sequential () const {
		if (m_node->is_dir) {
			return false;
		}
		if (!m_node->is_compressed) {
			if (m_node->original_size < zipfs_stream_file::OUTPUT_BUFFER_SIZE) {
				return false;
			}
		}
		return true;
	}

}