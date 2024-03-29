//
// Created by igor on 3/19/24.
//

#include <utility>
#include <istream>
#include <filesystem>
#include <cstring>
#include "lha_archive.hh"
#include "lhasa/public/lhasa.h"

namespace {

	std::filesystem::path result_path (const LHAFileHeader* header) {
		std::filesystem::path dir_path;
		if (header->path) {
			dir_path = std::filesystem::path (header->path);
		}
		std::filesystem::path file_path;
		if (header->filename) {
			file_path = std::filesystem::path (header->filename);
		}
		if (dir_path.empty ()) {
			return file_path;
		}
		if (file_path.is_absolute ()) {
			return file_path;
		}
		return dir_path / file_path;
	}
}

namespace vfs::extra {
	class lha_archive::entry_reader_impl {
	 public:
		explicit entry_reader_impl (std::istream& is);

		LHAFileHeader* next () {
			return lha_reader_next_file (m_reader);
		}

		[[nodiscard]] uint64_t data_pos () const {
			return m_stream.tellg ();
		}

		~entry_reader_impl ();
	 private:
		static int read (void* handle, void* buf, size_t buf_len) {
			auto* obj = reinterpret_cast<entry_reader_impl*>(handle);
			auto pos = obj->m_stream.tellg ();
			std::size_t remains = obj->m_file_size - (std::size_t )pos;
			std::size_t to_read = std::min(remains, buf_len);
			if (!obj->m_stream.read ((char*)buf, to_read)) {
				return -1;
			}
			auto curr = obj->m_stream.tellg ();
			return (size_t)curr - (size_t)pos;
		}

		static int skip (void* handle, size_t bytes) {
			auto* obj = reinterpret_cast<entry_reader_impl*>(handle);
			if (!obj->m_stream.seekg (bytes, std::ios::cur)) {
				return 0;
			}
			return 1;
		}

		static void close ([[maybe_unused]] void* handle) {

		}
	 private:
		std::istream& m_stream;
		LHAInputStreamType m_rdr_ops;
		LHAInputStream* m_lha_stream;
		LHAReader* m_reader;
		std::size_t m_file_size;
	};

	lha_archive::entry_reader_impl::entry_reader_impl (std::istream& is)
		: m_stream (is),
		  m_rdr_ops (LHAInputStreamType{read, skip, close}),
		  m_lha_stream (lha_input_stream_new (&m_rdr_ops, this)),
		  m_reader (lha_reader_new (m_lha_stream)) {
		auto current = is.tellg();
		is.seekg (0, std::ios::end);
		m_file_size = is.tellg() - current;
		is.seekg (0, std::ios::beg);
	}

	lha_archive::entry_reader_impl::~entry_reader_impl () {
		lha_reader_free (m_reader);
		lha_input_stream_free (m_lha_stream);
	}

	// =====================================================================================
	class lha_reader : public archive_reader <lha_entry> {
	 public:
		lha_reader(std::istream* is, const lha_entry& e)
		: m_pos(e.data_pos),
		  m_length(e.decompressed_size),
		  m_stream_length(e.compressed_size),
		  m_stream(is) {
			auto* decoder_type = lha_decoder_for_name (const_cast<char *>(e.compression_method.c_str()));
			m_decoder = lha_decoder_new (decoder_type, decoder_callback, this, e.decompressed_size);
		}

		~lha_reader() override {
			lha_decoder_free (m_decoder);
		}
	 private:
		std::size_t read ([[maybe_unused]] uint64_t input_stream_offset,
						 void* buff,
						 std::size_t size) override {
			std::size_t to_read = std::min(size, m_length);
			auto rc =  lha_decoder_read (m_decoder, (uint8_t*)buff, to_read);
			m_length -= rc;
			return rc;
		}
	 private:
		static std::size_t decoder_callback(void *buf, size_t buf_len,
											void *user_data) {
			auto* e = reinterpret_cast<lha_reader*>(user_data);
			std::size_t to_read = std::min(buf_len, e->m_stream_length);
			if (to_read > 0) {
				e->m_stream->seekg (e->m_pos, std::ios::beg);
				e->m_stream->read ((char*)buf, to_read);
				auto now = e->m_stream->tellg();
				auto has_bytes = (std::size_t)now - e->m_pos;
				e->m_pos += has_bytes;
				e->m_stream_length -= has_bytes;
				return has_bytes;
			}
			return 0;
		}
	 private:
		uint64_t m_pos;
		std::size_t m_length;
		std::size_t m_stream_length;
		std::istream* m_stream;
		LHADecoder*  m_decoder;
	};

	lha_archive::lha_archive (std::istream* is)
		: m_stream (is),
		  m_reader (nullptr) {}

	lha_archive::~lha_archive () = default;

	std::optional<lha_entry> lha_archive::next_entry () {
		if (!m_reader) {
			m_reader = std::make_unique<entry_reader_impl> (*m_stream);
		}
		while (true) {
			auto header = m_reader->next ();
			if (!header) {
				m_reader = nullptr;
				break;
			}
			auto pos = m_reader->data_pos ();
			bool is_dir = (strcmp (header->compress_method, LHA_COMPRESS_TYPE_DIR) == 0);
			if (header->symlink_target) {
				continue;
			}
			auto full_path = result_path (header).u8string ();
//			if (full_path.find ("fonts") != std::string::npos) {
//				int x = 0;
//			}
			return std::make_optional<lha_entry> (!is_dir, full_path, pos,
												  header->compressed_length,
												  header->length,
												  header->compress_method);
		}
		return std::nullopt;
	}

	std::unique_ptr<archive_reader<lha_entry>> lha_archive::create_reader (lha_entry& entry) {
		return std::make_unique<lha_reader>(m_stream, entry);
	}

	// =====================================================================================
	lha_entry::lha_entry (bool is_file,
						  std::string name,
						  uint64_t data_pos,
						  size_t compressed_size,
						  size_t decompressed_size,
						  const char* compression_method)
		: is_file (is_file),
		  name (std::move (name)),
		  data_pos (data_pos),
		  compressed_size (compressed_size),
		  decompressed_size (decompressed_size),
		  compression_method (compression_method) {}

}