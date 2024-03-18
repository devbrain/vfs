//
// Created by igor on 3/18/24.
//

#include "tarfile.hh"
#include "tar_stream.hh"
#include <bsw/exception.hh>
#include <bsw/strings/string_tokenizer.hh>

namespace vfs::extra {

	tarfile::tarfile (std::istream& is)
	: m_stream(is),
	m_tar(create_mtar_from_stream (&m_stream)),
	m_root(new tar_tree) {
		int rc = mtar_open_stream (m_tar.get(), "r");
		if (rc != MTAR_ESUCCESS) {
			RAISE_EX("mtar_open_stream: ", mtar_strerror(rc));
		}

		mtar_header_t h;
		while ( (mtar_read_header(m_tar.get(), &h)) != MTAR_ENULLRECORD ) {
			add_to_tree (h);
			rc = mtar_next(m_tar.get());
			if (rc != MTAR_ESUCCESS) {
				RAISE_EX("mtar_next: ", mtar_strerror(rc));
			}
		}
	}

	std::istream& tarfile::stream () {
		return m_stream;
	}

	const std::istream& tarfile::stream () const {
		return m_stream;
	}

	void tarfile::add_to_tree (const mtar_header_t& h) {
		tar_tree* current = m_root;

		bsw::string_tokenizer tokenizer(h.name, "/\\",
										bsw::string_tokenizer::TOK_IGNORE_EMPTY | bsw::string_tokenizer::TOK_TRIM);
		for (const auto & name : tokenizer) {
			if (name == ".") {
				continue;
			}
			auto i = current->children.find (name);
			if (i == current->children.end()) {
				auto* node = new tar_tree;
				current->children[name] = node;
				current = node;
			} else {
				current = i->second;
			}
		}
		current->is_dir = h.type == MTAR_TDIR;
		current->size = h.size;
		current->offset = h.data_pos;
	}

	tarfile::~tarfile () {
		mtar_close (m_tar.get());
		delete m_root;
	}

	const tar_tree* tarfile::get_root () const {
		return m_root;
	}

	tar_tree* tarfile::get_root () {
		return m_root;
	}

	tar_tree::tar_tree()
		: is_dir(true),
		  size(0),
		  offset(0) {}

	tar_tree::~tar_tree() {
		for (auto [k, v] : children) {
			delete v;
		}
	}
}