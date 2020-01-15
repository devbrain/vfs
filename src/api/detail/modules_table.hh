//
// Created by igor on 26/12/2019.
//

#ifndef MODULES_TABLE_HH
#define MODULES_TABLE_HH

#include "vfs/api/vfs_module.h"
#include "vfs/api/stdfilesystem.hh"
#include "api/detail/module_loader.hh"
#include "api/filesystem.hh"

#include <map>
#include <string>
#include <iterator>

namespace vfs
{
    class modules;
}

namespace vfs::core
{

	class modules_table
	{
        friend class vfs::modules;
	public:
		explicit modules_table(const stdfs::path& path);
		~modules_table();

		void add(const stdfs::path& path);
		[[nodiscard]] filesystem* get(const std::string& type) const;

	private:
		void _add_file(const stdfs::path& path);
	private:
		class entry
		{
			friend class modules_table;
		public:
			entry(vfs_module* obj, std::unique_ptr<shared_module>&& dll);

			void inc_ref();
			bool dec_ref();
			[[nodiscard]] int ref_count() const;
			[[nodiscard]] stdfs::path path() const;
			[[nodiscard]] filesystem* module() const;
		private:
			~entry();
		private:
			int _ref_count;
			filesystem* _fs;
			std::unique_ptr<shared_module> _dll;
		};
		friend class iterator;
		using map_t = std::map<std::string, entry*>;
		map_t _entries;
	public:

	};

} // ns vfs::core


#endif //MODULES_TABLE_HH
