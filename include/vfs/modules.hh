#ifndef VFS_MODULES_HH
#define VFS_MODULES_HH

#include <filesystem>
#include <memory>
#include <iosfwd>
#include <list>

#include <vfs/api/vfs_api.h>
#include <vfs/detail/wrapped_iterator.hh>

#include <bsw/warn/push.hh>
#include <bsw/warn/dll_interface>
#include <bsw/warn/dll_interface_base>

namespace vfs {
	namespace core {
		class modules_table;
	}

	struct VFS_API modules_loading_report_item {
		modules_loading_report_item (std::filesystem::path  path, const bool is_loaded);
		const std::filesystem::path path;
		const bool is_loaded;
	};

	using modules_loading_report = std::list<modules_loading_report_item>;

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

#include <bsw/warn/pop.hh>

#endif
