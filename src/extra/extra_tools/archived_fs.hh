//
// Created by igor on 3/28/24.
//

#ifndef VFS_SRC_EXTRA_EXTRA_TOOLS_ARCHIVED_FS_HH_
#define VFS_SRC_EXTRA_EXTRA_TOOLS_ARCHIVED_FS_HH_

#include <map>
#include <memory>
#include <string>
#include <optional>

#include <bsw/strings/string_tokenizer.hh>
#include "extra/extra_tools/read_only_fs.hh"
#include "extra/extra_tools/entry_props.hh"

namespace vfs::extra {

	template <class EntryProps>
	struct tree_entry : public EntryProps {
		tree_entry () = default;

		~tree_entry () {
			for (auto& [k, v] : children) {
				delete v;
			}
		}

		using children_t = std::map<std::string, tree_entry*>;
		children_t children;
	};

	template <class EntryProps>
	void add_to_tree (tree_entry<EntryProps>* root, const EntryProps entry) {
		auto* current = root;
		using traits = entry_props_traits<EntryProps>;
		bsw::string_tokenizer tokenizer (traits::get_file_name (entry), "/\\",
										 bsw::string_tokenizer::TOK_IGNORE_EMPTY | bsw::string_tokenizer::TOK_TRIM);
		for (const auto& name : tokenizer) {
			auto i = current->children.find (name);
			if (i == current->children.end ()) {
				auto* node = new tree_entry<EntryProps>;
				current->children[name] = node;
				current = node;
			} else {
				current = i->second;
			}
		}
		traits::update_tree_from_entry (*current, entry);
	}

	// ========================================================================================
	template <class EntryProps>
	class archive_reader {
	 public:
		virtual ~archive_reader() = default;

		virtual std::size_t read (uint64_t input_stream_offset,
								  void* buff,
								  std::size_t size) = 0;
	};

	// ========================================================================================
	template <class EntryProps>
	class archive_io {
	 public:
		archive_io () = default;
		virtual ~archive_io () = default;

		virtual std::optional<EntryProps> next_entry () = 0;
		virtual std::unique_ptr<archive_reader<EntryProps>> create_reader (EntryProps& entry) = 0;

	};

	// ========================================================================================
	template <class EntryProps, bool IsSequential>
	class archive_readonly_file : public read_only_file {
	 public:
		archive_readonly_file (archive_io<EntryProps>* io, tree_entry<EntryProps>* node)
			: m_archive (io),
			  m_pointer (0),
			  m_file_size (entry_props_traits<EntryProps>::get_archived_size (*node)),
			  m_reader (io->create_reader (*node)) {}

	 private:
		ssize_t read (void* buff, size_t len) override {
			size_t has_bytes = m_reader->read (m_pointer, buff, len);
			m_pointer += has_bytes;
			if (m_pointer > m_file_size) {
				m_pointer = m_file_size;
			}
			return static_cast<ssize_t>(has_bytes);
		}

		bool seek ([[maybe_unused]] uint64_t pos, [[maybe_unused]] whence_type whence) final {
			if constexpr (IsSequential) {
				return false;
			} else {
				if (pos > m_file_size) {
					return false;
				}
				auto new_offset = m_pointer;

				switch (whence) {
					case eVFS_SEEK_CUR: new_offset += pos;
						break;
					case eVFS_SEEK_SET: new_offset = pos;
						break;
					case eVFS_SEEK_END: new_offset = m_file_size - pos;
						break;
				}
				m_pointer = new_offset;
				return true;
			}
		}

		[[nodiscard]] uint64_t tell () const override {
			if constexpr (IsSequential) {
				return 0;
			}
			return m_pointer;
		}

	 private:
		const archive_io<EntryProps>* m_archive;
		uint64_t m_pointer;
		uint64_t m_file_size;
		std::unique_ptr<archive_reader<EntryProps>> m_reader;
	};

	// ========================================================================================
	template <class EntryProps, bool IsSequential>
	class archived_inode : public readonly_inode {
	 public:
		archived_inode (archive_io<EntryProps>* io, tree_entry<EntryProps>* node)
			: readonly_inode (entry_props_traits<EntryProps>::is_dir (*node), IsSequential),
			  m_io (io),
			  m_node (node) {
		}

	 private:
		class directory_iterator : public vfs::module::directory_iterator {
		 public:
			using children_itr_t = typename tree_entry<EntryProps>::children_t::const_iterator;

			directory_iterator (children_itr_t itr, children_itr_t end)
				: _itr (itr),
				  _end (end) {}

			bool has_next () override {
				return _itr != _end;
			}

			std::string next () override {
				std::string ret = _itr->first;
				_itr++;
				return ret;
			}

		 private:
			children_itr_t _itr;
			children_itr_t _end;
		};

	 private:
		readonly_inode* do_lookup (const char* name) override {
			auto itr = m_node->children.find (name);
			if (itr == m_node->children.end ()) {
				return nullptr;
			}
			return new archived_inode<EntryProps, IsSequential> (m_io, itr->second);
		}

		uint64_t size () override {
			return entry_props_traits<EntryProps>::get_size (*m_node);
		}

		vfs::module::directory_iterator* get_directory_iterator () override {
			return new directory_iterator (m_node->children.begin (), m_node->children.end ());
		}

		read_only_file* get_readonly_file () override {
			return new archive_readonly_file<EntryProps, IsSequential> (m_io, m_node);
		}

	 private:
		archive_io<EntryProps>* m_io;
		tree_entry<EntryProps>* m_node;
	};

	// ========================================================================================
	template <class EntryProps, bool IsSequential = true>
	class archived_fs : public readonly_fs {
	 public:
		explicit archived_fs (const char* fsname, size_t max_name_len = 256)
			: readonly_fs (fsname, max_name_len) {}

	 protected:
		virtual std::unique_ptr<archive_io<EntryProps>> create_archive_io (const std::string& params) = 0;

		readonly_inode* do_load_root (const std::string& params) override {
			m_archive = create_archive_io (params);
			EntryProps root;
			entry_props_traits<EntryProps>::update_tree_from_entry (m_root, root);
			while (true) {
				auto e = m_archive->next_entry ();
				if (!e) {
					break;
				}
				add_to_tree (&m_root, *e);
			}
			return new archived_inode<EntryProps, IsSequential> (m_archive.get (), &m_root);
		}

	 private:
		std::unique_ptr<archive_io<EntryProps>> m_archive;
		tree_entry<EntryProps> m_root;
	};

}

#endif //VFS_SRC_EXTRA_EXTRA_TOOLS_ARCHIVED_FS_HH_
