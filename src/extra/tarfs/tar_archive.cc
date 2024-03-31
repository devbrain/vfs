//
// Created by igor on 3/31/24.
//

#include <fstream>
#include "tar_archive.hh"
#include "tar_stream.hh"

namespace vfs::extra {

	class tar_reader : public archive_reader<tar_entry> {
	 public:
		tar_reader (std::istream* is, const tar_entry& e)
			: m_stream (is),
			  m_entry_offset (e.offset),
			  m_entry_size (e.size) {
		}

	 private:
		std::size_t read (uint64_t input_stream_offset,
						  void* buff,
						  std::size_t size) override {

			m_stream->seekg (m_entry_offset + input_stream_offset, std::ios::beg);

			std::size_t remains = m_entry_size - input_stream_offset;
			std::size_t to_read = std::min (size, remains);
			if (!m_stream->read ((char*)buff, to_read)) {
				RAISE_EX("tarfs I/O error");
			}
			auto current = m_stream->tellg ();
			return (uint64_t)current - (m_entry_offset + input_stream_offset);
		}

	 private:
		std::istream* m_stream;
		const uint64_t m_entry_offset;
		const std::size_t m_entry_size;
	};

	tar_archive::tar_archive (const std::string& params) {
		m_stream = std::make_unique<std::ifstream> (params, std::ios::binary | std::ios::in);
		if (!m_stream->good ()) {
			RAISE_EX("tarfs: Failed to open file ", params);
		}
		m_tar = create_mtar_from_stream (m_stream.get ());
		int rc = mtar_open_stream (m_tar.get (), "r");
		if (rc != MTAR_ESUCCESS) {
			RAISE_EX("mtar_open_stream: ", mtar_strerror (rc));
		}
	}

	tar_archive::~tar_archive () {
		mtar_close (m_tar.get ());
	}

	std::optional<tar_entry> tar_archive::next_entry () {

		mtar_header_t h;
		int rc = mtar_read_header (m_tar.get (), &h);
		if (rc != MTAR_ESUCCESS) {
			if (rc == MTAR_ENULLRECORD) {
				return std::nullopt;
			}
			RAISE_EX("mtar_read_header: ", mtar_strerror (rc));
		}
		tar_entry e;
		e.is_dir = h.type == MTAR_TDIR;
		e.size = h.size;
		e.offset = h.data_pos;
		e.name = h.name;
		rc = mtar_next (m_tar.get ());
		if (rc != MTAR_ESUCCESS) {
			RAISE_EX("mtar_next: ", mtar_strerror (rc));
		}
		return e;
	}

	std::unique_ptr<archive_reader<tar_entry>> tar_archive::create_reader (tar_entry& entry) {
		return std::make_unique<tar_reader> (m_stream.get (), entry);
	}
}