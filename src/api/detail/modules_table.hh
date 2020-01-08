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

namespace vfs::core
{

	class modules_table
	{
	public:
		class iterator;
	public:
		explicit modules_table(const stdfs::path& path);
		~modules_table();

		void add(const stdfs::path& path);
		[[nodiscard]] iterator begin();
		[[nodiscard]] iterator end();
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
		class iterator
		{
			friend class modules_table;
		public:
			class data
			{
				friend class iterator;
			private:
				std::string _type;
				int _refcount;
				stdfs::path _path;
			public:
				[[nodiscard]] std::string type() const;
				[[nodiscard]] int refcount() const;
				[[nodiscard]] stdfs::path path() const;
			private:
				explicit data(const map_t::const_iterator& itr);
			};
		public:
			typedef data value_type;
			typedef std::ptrdiff_t difference_type;
			typedef data* pointer;
			typedef data& reference;
			typedef std::input_iterator_tag iterator_category;
		private:
			map_t::const_iterator _value;
			explicit iterator(map_t::const_iterator itr)
				: _value(itr)
			{
			}
		public:
			[[nodiscard]] data operator*() const;

			bool operator==(const iterator& other) const;
			bool operator!=(const iterator& other) const;

			data operator++(int);
			iterator& operator++();

		};
	};

} // ns vfs::core


#endif //MODULES_TABLE_HH
