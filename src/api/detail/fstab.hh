#ifndef VFS_DETAIL_FSTAB_HH
#define VFS_DETAIL_FSTAB_HH


#include "file_system.hh"
#include "mount_point.hh"
#include "api/detail/wrapped_pointer.hh"

#include "path.hh"

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
			entry (file_system* fs, const path& pth, const std::string& args);
			entry(entry&& other) = default;
			~entry();

			[[nodiscard]] path mount_path() const noexcept;
            [[nodiscard]] std::string args () const noexcept;
            [[nodiscard]] std::string type () const noexcept;
            [[nodiscard]] bool is_readonly () const noexcept;

            [[nodiscard]] wrapped_pointer<mount_point> get() const;
		private:
			std::unique_ptr<mount_point> _mount_point;
			std::string _type;
			std::string _args;
			path _path;
			file_system* _fs;
		};

		wrapped_pointer<mount_point> mount (file_system* module, const path& mount_path, const std::string& args);
		void unmount (const path& mount_path);
	private:
		using entry_map_t = std::map<std::size_t, entry>;
		entry_map_t _fstab;
	};
} // ns vfs

#endif
