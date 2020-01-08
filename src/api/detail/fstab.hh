#ifndef VFS_DETAIL_FSTAB_HH
#define VFS_DETAIL_FSTAB_HH

#include "api/hasher.hh"
#include "api/filesystem.hh"
#include "api/mount_point.hh"

#include "vfs/api/path.hh"

#include <memory>
#include <map>

namespace vfs::core
{
	class fstab
	{
	public:
		class entry
		{
		public:
			entry(filesystem* fs, const path& pth, const std::string& args);
			~entry();

			path mount_path() const noexcept;
			std::string args () const noexcept;
			std::string type () const noexcept;
		private:
			std::unique_ptr<mount_point> _mount_point;
			std::string _type;
			std::string _args;
			path _path;
		};

		void mount(filesystem* module, const path& mount_path, const std::string& args);
		void unmount (const path& mount_path);
	private:
		using entry_map_t = std::map<core::hash_t, entry>;
		entry_map_t _fstab;
	};
} // ns vfs

#endif
