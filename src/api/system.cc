//
// Created by igor on 26/12/2019.
//

#include <iostream>

#include "vfs/api/system.hh"
#include "api/detail/modules_table.hh"

namespace vfs
{
	namespace detail
	{
		struct system::impl
		{
			impl(const stdfs::path& modules_path)
			: all_modules(modules_path)
			{
				for (const auto& v : all_modules)
				{
					std::cout << v.type() << "\t" << v.path() << std::endl;

				}
			}
			modules_table all_modules;
		};
		// =============================================================================
		system::system(const stdfs::path& modules_path)
		{
			_impl = std::make_unique<impl>(modules_path);
		}
		// ------------------------------------------------------------------------------
		system::~system() = default;
	} // ns detail
} // ns vfs
