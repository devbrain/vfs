#ifndef VFS_DETAIL_FSTAB_HH
#define VFS_DETAIL_FSTAB_HH

#include "api/hasher.hh"
#include "api/filesystem.hh"
#include "api/mount_point.hh"
#include "api/detail/wrapped_pointer.hh"

#include "vfs/api/path.hh"

#include <memory>
#include <map>

namespace vfs
{
    class mounts;
}

namespace vfs::core
{
	class fstab
	{
	    friend class vfs::mounts;
	public:
		class entry
		{
		public:
			entry(filesystem* fs, const path& pth, const std::string& args);
			~entry();

			path mount_path() const noexcept;
			std::string args () const noexcept;
			std::string type () const noexcept;

			wrapped_pointer<mount_point> get() const;
		private:
			std::unique_ptr<mount_point> _mount_point;
			std::string _type;
			std::string _args;
			path _path;
		};

		wrapped_pointer<mount_point> mount(filesystem* module, const path& mount_path, const std::string& args);
		void unmount (const path& mount_path);
	private:
		using entry_map_t = std::map<core::hash_t, entry>;
		entry_map_t _fstab;
	};
} // ns vfs

#endif
