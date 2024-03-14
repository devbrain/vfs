//
// Created by igor on 3/5/24.
//

#include "floppyfs_inode.hh"
#include "floppyfs_file.hh"

namespace vfs::extra {

	class directory_iterator : public vfs::module::directory_iterator {
	 public:
		using children_itr_t = std::vector<std::string>::const_iterator;
		explicit directory_iterator(std::vector<std::string>&& names)
			: m_names(std::move(names)),
			  m_itr(m_names.begin()) {}

		bool has_next () override {
			return m_itr != m_names.end();
		}

		std::string next () override {
			std::string ret = *m_itr;
			m_itr++;
			return ret;
		}
	 private:
		std::vector<std::string> m_names;
		children_itr_t m_itr;
	};


	floppyfs_inode::floppyfs_inode (fat12* fat, uint16_t cluster, bool is_dir, uint32_t size)
		: vfs::module::inode (is_dir ? VFS_INODE_DIRECTORY : VFS_INODE_REGULAR),
		  m_fat (fat),
		  m_cluster (cluster),
		  m_size (size) {
	}

	module::inode* floppyfs_inode::lookup (const char* name) {
		uint16_t ino_cluster = 0;
		bool ino_is_dir = false;
		uint32_t ino_size = 0;
		m_fat->read_dir (m_cluster, [&ino_cluster, &ino_is_dir, &ino_size, name] (uint16_t cluster,
											  bool is_dir,
											  uint32_t size,
											  const std::string& ename) {
		  if (ename == name) {
			  ino_cluster = cluster;
			  ino_is_dir = is_dir;
			  ino_size = size;
			  return false;
		  }
		  return true;
		});
		if (m_cluster != 0) {
			return new floppyfs_inode (m_fat, ino_cluster, ino_is_dir, ino_size);
		}
		return nullptr;
	}

	uint64_t floppyfs_inode::size () {
		return m_size;
	}

	vfs::module::directory_iterator* floppyfs_inode::get_directory_iterator () {
		std::vector<std::string> names;
		m_fat->read_dir (m_cluster, [&names] ([[maybe_unused]]uint16_t cluster,
											  [[maybe_unused]]bool is_dir,
											  [[maybe_unused]]uint32_t size, const std::string& name) {
		  names.emplace_back (name);
		  return true;
		});
		return new directory_iterator(std::move (names));
	}

	bool floppyfs_inode::mkdir (const char* name) {
		return false;
	}

	bool floppyfs_inode::mkfile (const char* name) {
		return false;
	}

	int floppyfs_inode::unlink () {
		return 0;
	}

	bool floppyfs_inode::is_sequential () const {
		return false;
	}

	vfs::module::file* floppyfs_inode::open_file ([[maybe_unused]] open_mode_type mode_type) {
		return new floppyfs_file (m_fat, m_cluster);
	}
}