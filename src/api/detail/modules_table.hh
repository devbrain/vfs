//
// Created by igor on 26/12/2019.
//

#ifndef MODULES_TABLE_HH
#define MODULES_TABLE_HH

#include "vfs/api/vfs_module.h"
#include "vfs/api/stdfilesystem.hh"
#include "api/detail/module_loader.hh"
#include <map>
#include <string>

namespace vfs
{
	namespace detail
	{
		class modules_table
		{
		public:
			explicit modules_table(const stdfs::path& path);
			void add(const stdfs::path& path);
		private:
			void _add_file(const stdfs::path& path);
		private:
			class entry
			{
			public:
				entry(vfs_module* obj, std::unique_ptr<shared_module>&& dll);

				void inc_ref();
				bool dec_ref();
			private:
				~entry();
			private:
				int _ref_count;
				vfs_module* _module;
				std::unique_ptr<shared_module> _dll;
			};

			std::map<std::string, entry*> _entries;
		};
	} // ns detail
} // ns vfs


#endif //MODULES_TABLE_HH
