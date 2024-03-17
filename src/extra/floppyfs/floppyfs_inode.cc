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


	floppyfs_inode::floppyfs_inode (driver* fat, uint16_t cluster, bool is_dir, uint32_t size)
		: vfs::module::inode (is_dir ? VFS_INODE_DIRECTORY : VFS_INODE_REGULAR),
		  m_fat (fat),
		  m_cluster (cluster),
		  m_size (size) {
	}

	module::inode* floppyfs_inode::lookup (const char* name) {
		auto fat_dir = m_fat->get_directory (m_cluster);
		auto itr = fat_dir.get_iterator();
		while (true) {
			auto opt_entry = itr.read();
			if (opt_entry) {
				if (opt_entry->name == name) {
					return new floppyfs_inode (m_fat, opt_entry->cluster, opt_entry->is_dir, opt_entry->size);
				}
			} else {
				break;
			}
		}
		return nullptr;
	}

	uint64_t floppyfs_inode::size () {
		return m_size;
	}

	vfs::module::directory_iterator* floppyfs_inode::get_directory_iterator () {
		std::vector<std::string> names;
		auto fat_dir = m_fat->get_directory (m_cluster);
		auto itr = fat_dir.get_iterator();
		while (true) {
			auto opt_entry = itr.read();
			if (opt_entry) {
				names.emplace_back (opt_entry->name);
			} else {
				break;
			}
		}
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