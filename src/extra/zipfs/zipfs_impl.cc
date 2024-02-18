//
// Created by igor on 2/4/24.
//

#include "zipfs_impl.hh"
#include <vfs/extra/zipfs.hh>
#include <fstream>
#include "popl.hpp"

namespace vfs::extra {

	struct cmds {
		bool is_vfs;
		std::string path;
	};

	static cmds parse_commands(const std::string& args) {
		using namespace popl;
		OptionParser op("Allowed options");
		auto vfs_option   = op.add<Switch>("v", "vfs", "treat path as vfs");

		op.parse ("zipfs", args);

		if (op.non_option_args().size() != 1) {
			throw std::runtime_error("path to the zip file should be specified only once");
		}

		return {
			vfs_option->is_set(),
			*op.non_option_args().begin()
		};
	}

	zipfs::zipfs ()
		: vfs::module::filesystem ("zipfs"),
		  m_stream (nullptr) {
	}

	vfs::module::inode* zipfs::load_root (const std::string& params) {
		auto cmds = parse_commands (params);
		if (!cmds.is_vfs) {
			m_stream = new std::ifstream (cmds.path, std::ios::binary | std::ios::in);
		}
		return nullptr;
	}

	size_t zipfs::max_name_length () {
		return 64;
	}

	int zipfs::sync () {
		return 1;
	}

	int zipfs::sync_inode (vfs::module::inode* inod) {
		return 1;
	}

	[[nodiscard]] bool zipfs::is_readonly () const {
		return true;
	}

	zipfs::~zipfs () {
		delete m_stream;
	}

	std::unique_ptr<vfs::module::filesystem> create_zipfs() {
		return std::make_unique<zipfs>();
	}
}

#if !defined(ZIPFS_STATIC_DEFINE)
REGISTER_VFS_MODULE(vfs::extra::zipfs)
#endif

