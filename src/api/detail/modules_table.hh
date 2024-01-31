//
// Created by igor on 26/12/2019.
//

#ifndef MODULES_TABLE_HH
#define MODULES_TABLE_HH

#include <vfs/api/vfs_module.h>
#include <filesystem>
#include "api/detail/module_loader.hh"
#include "file_system.hh"

#include <map>
#include <string>
#include <iterator>

namespace vfs {
	class modules;
}

namespace vfs::core {

	class modules_table {
		friend class vfs::modules;

	 public:
		modules_table();
		explicit modules_table (const std::filesystem::path& path);
		explicit modules_table(std::unique_ptr<vfs::module::filesystem> fsptr);
		~modules_table ();

		void add (const std::filesystem::path& path);
		void add (std::unique_ptr<vfs::module::filesystem> fsptr);
		[[nodiscard]] file_system* get (const std::string& type) const;

	 private:
		void _add_module(vfs::module::filesystem* fs);
		void _add_file (const std::filesystem::path& path);
	 private:
		class entry {
			friend class modules_table;

		 public:
			entry (vfs_module* obj, std::unique_ptr<shared_module>&& dll);

			void inc_ref ();
			bool dec_ref ();
			[[nodiscard]] int ref_count () const;
			[[nodiscard]] std::filesystem::path path () const;
			[[nodiscard]] file_system* module () const;
		 private:
			~entry ();
		 private:
			int _ref_count;
			file_system* _fs;
			std::unique_ptr<shared_module> _dll;
		};

		friend class iterator;

		using map_t = std::map<std::string, entry*>;
		map_t _entries;
	 public:

	};

} // ns vfs::core


#endif //MODULES_TABLE_HH
