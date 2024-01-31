#ifndef VFS_MODULES_HH
#define VFS_MODULES_HH

#include "vfs/api/vfs_api.h"
#include <filesystem>
#include "vfs/api/detail/wrapped_iterator.hh"
#include <memory>
#include <iosfwd>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning( disable : 4275 4251 )
#endif
namespace vfs {
	namespace core {
		class modules_table;
	}

	class VFS_API modules;

	VFS_API modules get_modules ();

	class VFS_API modules {
		friend VFS_API modules get_modules ();
		struct wrapper;
	 public:
		class iterator;

	 public:

		~modules ();
		[[nodiscard]] iterator begin ();
		[[nodiscard]] iterator end ();
	 private:
		explicit modules (core::modules_table* mtable);
		struct impl;

		std::unique_ptr<impl> _impl;

	 public:
		class VFS_API data {
			friend class detail::iterator<data, wrapper>;

		 private:
			std::string _type;
			int _refcount;
			std::filesystem::path _path;
		 public:
			[[nodiscard]] std::string type () const noexcept;
			[[nodiscard]] int refcount () const noexcept;
			[[nodiscard]] std::filesystem::path path () const noexcept;
		 private:
			explicit data (const wrapper& itr);
		};

		class iterator : public detail::iterator<data, wrapper> {
			friend class modules_table;

			friend class modules;

			explicit iterator (wrapper& itr)
				: detail::iterator<data, wrapper> (itr) {
			}

			bool _equals (const wrapper& a, const wrapper& b) const noexcept override;
			void _inc (wrapper& a) override;
		};
	};

	VFS_API modules::iterator begin (modules& m);
	VFS_API modules::iterator end (modules& m);

	VFS_API std::ostream& operator<< (std::ostream&, const modules::data&);
} // ns vfs

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
