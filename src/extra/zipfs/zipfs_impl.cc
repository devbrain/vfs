//
// Created by igor on 2/4/24.
//

#include <fstream>

#include <vfs/extra/zipfs.hh>

#include "popl.hpp"

#include "zipfs_inode.hh"
#include "zipfs_impl.hh"

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
		  m_archive (nullptr) {
	}

	vfs::module::inode* zipfs::load_root (const std::string& params) {
		auto cmds = parse_commands (params);
		if (!cmds.is_vfs) {
			auto stream = std::make_unique<std::ifstream> (cmds.path, std::ios::binary | std::ios::in);
			stream->seekg (0, std::ios::end);
			auto size = stream->tellg();
			stream->seekg (0, std::ios::beg);
			m_archive = std::make_unique<zip_archive>(std::move(stream), size);
		}
		return new zipfs_inode(m_archive.get(), m_archive->get_root());
	}

	size_t zipfs::max_name_length () {
		return 512;
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

	zipfs::~zipfs () = default;

	std::unique_ptr<vfs::module::filesystem> create_zipfs() {
		return std::make_unique<zipfs>();
	}
}

#if !defined(ZIPFS_STATIC_DEFINE)
REGISTER_VFS_MODULE(vfs::extra::zipfs)
#endif

