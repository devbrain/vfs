#ifndef VFS_MOUNTS_HH
#define VFS_MOUNTS_HH

#include <vfs/api/vfs_api.h>
#include <filesystem>
#include <vfs/detail/wrapped_iterator.hh>
#include <memory>
#include <iosfwd>

#include <bsw/warn/push.hh>
#include <bsw/warn/dll_interface>
#include <bsw/warn/dll_interface_base>


namespace vfs {
	namespace core {
		class fstab;
	}

	class VFS_API mounts;

	VFS_API mounts get_mounts ();

	class VFS_API mounts {
		friend VFS_API mounts get_mounts ();
		struct wrapper;
	 public:
		class iterator;

	 public:

		~mounts ();
		[[nodiscard]] iterator begin ();
		[[nodiscard]] iterator end ();
	 private:
		explicit mounts (core::fstab* mtable);
		struct impl;
		std::unique_ptr<impl> _impl;
	 public:
		class VFS_API data {
			friend class detail::iterator<data, wrapper>;

		 private:
			std::string _path;
			std::string _type;
			std::string _args;
			bool _readonly;
		 public:

			[[nodiscard]] std::string type () const noexcept;
			[[nodiscard]] std::string args () const noexcept;
			[[nodiscard]] std::string path () const noexcept;
			[[nodiscard]] bool is_readonly () const noexcept;
		 private:

			explicit data (const wrapper& itr);
		};

	 public:
		class iterator : public detail::iterator<data, wrapper> {
			friend class fstab;

			friend class mounts;

		 private:
			explicit iterator (wrapper& itr)
				: detail::iterator<data, wrapper> (itr) {
			}

			bool _equals (const wrapper& a, const wrapper& b) const noexcept override;
			void _inc (wrapper& a) override;

		};
	};

	VFS_API mounts::iterator begin (mounts& m);
	VFS_API mounts::iterator end (mounts& m);

	VFS_API std::ostream& operator<< (std::ostream&, const mounts::data&);
} // ns vfs

#include <bsw/warn/pop.hh>

#endif
