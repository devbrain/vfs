//
// Created by igor on 26/12/2019.
//

#include <cstdlib>

#include "vfs/api/system.hh"
#include "api/detail/modules_table.hh"

namespace vfs
{
	namespace detail
	{
		class system
		{
		public:
			explicit system(const stdfs::path& modules_path);
			void add_module(const stdfs::path& modules_path);

			[[nodiscard]] vfs_module* get_module(const std::string& type) const;

			~system();
		private:
			modules_table _all_modules;
		};
		// =============================================================================
		system::system(const stdfs::path& modules_path)
			: _all_modules(modules_path)
		{
		}
		// ------------------------------------------------------------------------------
		void system::add_module(const stdfs::path& modules_path)
		{
			_all_modules.add(modules_path);
		}
		// ------------------------------------------------------------------------------
		vfs_module* system::get_module(const std::string& type) const
		{
			return _all_modules.get(type);
		}
		// ------------------------------------------------------------------------------
		system::~system() = default;
	} // ns detail
	// ==================================================================================
	static detail::system* system = nullptr;
	static void system_destructor()
	{
		delete system;
	}
	// ----------------------------------------------------------------------------------
	void load_module(const stdfs::path& path_to_module)
	{
		if (system == nullptr)
		{
			system = new detail::system(path_to_module);
			std::atexit(system_destructor);
		}
		else
		{
			system->add_module(path_to_module);
		}
	}
} // ns vfs
