//
// Created by igor on 2/18/24.
//

#include <istream>
#include <algorithm>
#include <bsw/exception.hh>
#include <bsw/logger/logger.hh>
#include "zip_archive.hh"
#include "extra/extra_tools/archived_fs.hh"

static size_t file_read_func (void* pOpaque, mz_uint64 file_ofs, void* pBuf, size_t n) {
	auto* is = reinterpret_cast<std::istream*>(pOpaque);
	is->seekg (static_cast<std::streamoff>(file_ofs), std::ios::beg);
	is->read ((char*)pBuf, static_cast<std::streamsize>(n));
	return is->gcount ();
}

static void raise_last_error (const mz_zip_archive& a) {
	switch (a.m_last_error) {
		case MZ_ZIP_NO_ERROR : return;
		case MZ_ZIP_UNDEFINED_ERROR : RAISE_EX("MZ_ZIP_UNDEFINED_ERROR");
		case MZ_ZIP_TOO_MANY_FILES : RAISE_EX("MZ_ZIP_TOO_MANY_FILES");
		case MZ_ZIP_FILE_TOO_LARGE : RAISE_EX("MZ_ZIP_FILE_TOO_LARGE");
		case MZ_ZIP_UNSUPPORTED_METHOD : RAISE_EX("MZ_ZIP_UNSUPPORTED_METHOD");
		case MZ_ZIP_UNSUPPORTED_ENCRYPTION : RAISE_EX("MZ_ZIP_UNSUPPORTED_ENCRYPTION");
		case MZ_ZIP_UNSUPPORTED_FEATURE : RAISE_EX("MZ_ZIP_UNSUPPORTED_FEATURE");
		case MZ_ZIP_FAILED_FINDING_CENTRAL_DIR : RAISE_EX("MZ_ZIP_FAILED_FINDING_CENTRAL_DIR");
		case MZ_ZIP_NOT_AN_ARCHIVE : RAISE_EX("MZ_ZIP_NOT_AN_ARCHIVE");
		case MZ_ZIP_INVALID_HEADER_OR_CORRUPTED : RAISE_EX("MZ_ZIP_INVALID_HEADER_OR_CORRUPTED");
		case MZ_ZIP_UNSUPPORTED_MULTIDISK : RAISE_EX("MZ_ZIP_UNSUPPORTED_MULTIDISK");
		case MZ_ZIP_DECOMPRESSION_FAILED : RAISE_EX("MZ_ZIP_DECOMPRESSION_FAILED");
		case MZ_ZIP_COMPRESSION_FAILED : RAISE_EX("MZ_ZIP_COMPRESSION_FAILED");
		case MZ_ZIP_UNEXPECTED_DECOMPRESSED_SIZE : RAISE_EX("MZ_ZIP_UNEXPECTED_DECOMPRESSED_SIZE");
		case MZ_ZIP_CRC_CHECK_FAILED : RAISE_EX("MZ_ZIP_CRC_CHECK_FAILED");
		case MZ_ZIP_UNSUPPORTED_CDIR_SIZE : RAISE_EX("MZ_ZIP_UNSUPPORTED_CDIR_SIZE");
		case MZ_ZIP_ALLOC_FAILED : RAISE_EX("MZ_ZIP_ALLOC_FAILED");
		case MZ_ZIP_FILE_OPEN_FAILED : RAISE_EX("MZ_ZIP_FILE_OPEN_FAILED");
		case MZ_ZIP_FILE_CREATE_FAILED : RAISE_EX("MZ_ZIP_FILE_CREATE_FAILED");
		case MZ_ZIP_FILE_WRITE_FAILED : RAISE_EX("MZ_ZIP_FILE_WRITE_FAILED");
		case MZ_ZIP_FILE_READ_FAILED : RAISE_EX("MZ_ZIP_FILE_READ_FAILED");
		case MZ_ZIP_FILE_CLOSE_FAILED : RAISE_EX("MZ_ZIP_FILE_CLOSE_FAILED");
		case MZ_ZIP_FILE_SEEK_FAILED : RAISE_EX("MZ_ZIP_FILE_SEEK_FAILED");
		case MZ_ZIP_FILE_STAT_FAILED : RAISE_EX("MZ_ZIP_FILE_STAT_FAILED");
		case MZ_ZIP_INVALID_PARAMETER : RAISE_EX("MZ_ZIP_INVALID_PARAMETER");
		case MZ_ZIP_INVALID_FILENAME : RAISE_EX("MZ_ZIP_INVALID_FILENAME");
		case MZ_ZIP_BUF_TOO_SMALL : RAISE_EX("MZ_ZIP_BUF_TOO_SMALL");
		case MZ_ZIP_INTERNAL_ERROR : RAISE_EX("MZ_ZIP_INTERNAL_ERROR");
		case MZ_ZIP_FILE_NOT_FOUND : RAISE_EX("MZ_ZIP_FILE_NOT_FOUND");
		case MZ_ZIP_ARCHIVE_TOO_LARGE : RAISE_EX("MZ_ZIP_ARCHIVE_TOO_LARGE");
		case MZ_ZIP_VALIDATION_FAILED : RAISE_EX("MZ_ZIP_VALIDATION_FAILED");
		case MZ_ZIP_WRITE_CALLBACK_FAILED : RAISE_EX("MZ_ZIP_WRITE_CALLBACK_FAILED");
		default: RAISE_EX("Unknown error");
	}

}

static uint64_t get_data_offset (mz_zip_archive& archive, const mz_zip_archive_file_stat& s) {
	constexpr uint16_t MZ_ZIP_LOCAL_DIR_HEADER_SIZE = 30;
	mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof (mz_uint32) - 1) / sizeof (mz_uint32)];
	auto* pLocal_header = (mz_uint8*)local_header_u32;
	auto cur_file_ofs = s.m_local_header_ofs;
	if (archive.m_pRead (archive.m_pIO_opaque, cur_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
		!= MZ_ZIP_LOCAL_DIR_HEADER_SIZE) {
		RAISE_EX("Failed to read zip file header for index ", s.m_file_index);
	}

	constexpr uint32_t MZ_ZIP_LOCAL_DIR_HEADER_SIG = 0x04034b50;

	if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG) {
		RAISE_EX("Zip header is corrupted for entry at index ", s.m_file_index);
	}
	constexpr uint32_t MZ_ZIP_LDH_FILENAME_LEN_OFS = 26;
	constexpr uint32_t MZ_ZIP_LDH_EXTRA_LEN_OFS = 28;
	cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS)
					+ MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
	return cur_file_ofs;
}

namespace vfs::extra {

	class uncompressed_archive_reader : public archive_reader<zip_entry> {
	 public:
		uncompressed_archive_reader (mz_zip_archive& archive, const zip_entry& e)
			: m_archive   (archive),
			  m_offset    (e.offset),
			  m_pointer   (0),
			  m_file_size (e.original_size) {}

	 private:
		std::size_t read ([[maybe_unused]] uint64_t input_stream_offset,
						  void* buff,
						  std::size_t size) override {
			auto to_read = std::min(static_cast<size_t>(m_file_size - m_pointer), size);
			auto has_bytes = m_archive.m_pRead (m_archive.m_pIO_opaque, m_offset + m_pointer, buff, to_read);
			if (has_bytes != to_read) {
				RAISE_EX("zipfs I/O error");
			}
			m_pointer += to_read;
			return static_cast<ssize_t>(to_read);
		}
	 protected:
		std::size_t raw_read(void* buff, std::size_t len, uint64_t offset) const {
			auto has_bytes = m_archive.m_pRead (m_archive.m_pIO_opaque, offset, buff, len);
			return has_bytes;
		}
	 protected:
		mz_zip_archive& m_archive;
		uint64_t    m_offset;
		uint64_t    m_pointer;
		std::size_t m_file_size;
	};

	class compressed_archive_reader : public uncompressed_archive_reader {
	 public:
		static constexpr std::size_t INPUT_BUFFER_SIZE = 64 * 1024;
		static constexpr std::size_t OUTPUT_BUFFER_SIZE = 64 * 1024;
	 public:
		compressed_archive_reader (mz_zip_archive& archive, const zip_entry& node)
		: uncompressed_archive_reader (archive, node),
		m_infile_remaining (node.compressed_size),
		m_next_out (nullptr),
		m_avail_out (0),
		m_total_out (0),
		m_next_in (nullptr),
		m_avail_in (0),
		m_total_in (0),
		m_outbuf_len (0),
		m_outbuf_ptr (0) {
			tinfl_init(&m_inflator);
			m_next_out = m_outbuf;
			m_avail_out = OUTPUT_BUFFER_SIZE;
		}

	 private:
		std::size_t read ([[maybe_unused]] uint64_t input_stream_offset,
						  void* buff,
						  std::size_t size) override {
			if (m_pointer >= m_file_size) {
				return 0;
			}

			auto bytes_in_obuf = m_outbuf_len - m_outbuf_ptr;
			if (bytes_in_obuf < size) {
				auto rc = read_chunk ();
				if (rc < 0) {
					return rc;
				}
				bytes_in_obuf = (std::size_t)rc;
				m_outbuf_len = bytes_in_obuf;
				m_outbuf_ptr = 0;
			}
			auto can_read = std::min (bytes_in_obuf, size);
			std::memcpy (buff, m_outbuf + m_outbuf_ptr, can_read);
			m_outbuf_ptr += can_read;
			m_pointer += can_read;
			return (ssize_t)can_read;
		}

		ssize_t read_chunk () {
			size_t in_bytes, out_bytes;
			tinfl_status status;
			if (!m_avail_in) {
				// Input buffer is empty, so read more bytes from input file.
				auto to_read = std::min (INPUT_BUFFER_SIZE, m_infile_remaining);
				auto n = raw_read (m_inbuf, to_read, m_offset + m_total_in);
				m_next_in = m_inbuf;
				m_avail_in = n;
				m_infile_remaining -= n;
			}
			in_bytes = m_avail_in;
			out_bytes = m_avail_out;
			status = tinfl_decompress (&m_inflator, (const mz_uint8*)m_next_in, &in_bytes, m_outbuf, m_next_out, &out_bytes, (
				m_infile_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0));

			m_avail_in -= in_bytes;
			m_next_in = m_next_in + in_bytes;
			m_total_in += in_bytes;

			m_avail_out -= out_bytes;
			m_next_out = m_next_out + out_bytes;
			m_total_out += out_bytes;

			std::size_t out_buff_size = 0;
			if ((status <= TINFL_STATUS_DONE) || (!m_avail_out)) {
				// Output buffer is full, or decompression is done, so write buffer to output file.
				out_buff_size = OUTPUT_BUFFER_SIZE - m_avail_out;
				m_next_out = m_outbuf;
				m_avail_out = OUTPUT_BUFFER_SIZE;

			}

			// If status is <= TINFL_STATUS_DONE then either decompression is done or something went wrong.
			if (status <= TINFL_STATUS_DONE) {
				if (status == TINFL_STATUS_DONE) {
					// Decompression completed successfully.
					return (ssize_t)out_buff_size;
				} else {
					return -1;
				}
			}
			return (ssize_t)out_buff_size;
		}

	 private:
		uint64_t m_infile_remaining;

		mz_uint8* m_next_out;
		uint64_t  m_avail_out;
		size_t    m_total_out;

		mz_uint8*  m_next_in;
		size_t     m_avail_in;
		size_t     m_total_in;
		tinfl_decompressor m_inflator{};
		unsigned char m_inbuf[INPUT_BUFFER_SIZE]{};
		unsigned char m_outbuf[OUTPUT_BUFFER_SIZE]{};

		std::size_t m_outbuf_len;
		std::size_t m_outbuf_ptr;

	};

	zip_archive::zip_archive (std::unique_ptr<std::istream> istream, uint64_t file_size)
		: m_istream (std::move (istream)),
		  m_file_size (file_size),
		  m_num_files (0),
		  m_curr_file (0),
		  m_archive{} {

		mz_zip_zero_struct (&m_archive);
		m_archive.m_pIO_opaque = m_istream.get ();
		m_archive.m_pRead = file_read_func;
		auto rc = mz_zip_reader_init (&m_archive, file_size, 0);
		if (rc == MZ_FALSE) {
			raise_last_error (m_archive);
		}
		m_num_files = mz_zip_reader_get_num_files(&m_archive);
	}

	std::optional<zip_entry> zip_archive::next_entry () {
		while (m_curr_file < m_num_files) {
			mz_zip_archive_file_stat file_stat;
			if (!mz_zip_reader_file_stat (&m_archive, m_curr_file, &file_stat)) {
				raise_last_error (m_archive);
			}
			m_curr_file++;
			constexpr uint16_t MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED = 1;
			constexpr uint16_t MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_COMPRESSED_PATCH_FLAG = 32;
			constexpr uint16_t MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION = 64;

			if (file_stat.m_bit_flag
				& (MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION
				   | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_COMPRESSED_PATCH_FLAG)) {
				EVLOG_TRACE(EVLOG_WARNING, "Encrypted files are unsupported");
				continue;
			}
			uint64_t offset = m_archive.m_archive_size;
			bool is_compressed = false;
			if (!file_stat.m_is_directory) {
				offset = get_data_offset (m_archive, file_stat);
				is_compressed = file_stat.m_uncomp_size != file_stat.m_comp_size;
				auto input_size = is_compressed ? file_stat.m_comp_size : file_stat.m_uncomp_size;
				if (offset + input_size > m_archive.m_archive_size) {
					EVLOG_TRACE(EVLOG_WARNING, "Corrupted header for zip file entry at index ", file_stat.m_file_index);
					continue;
				}
			}
			zip_entry e;
			e.is_dir = file_stat.m_is_directory;
			e.comments = file_stat.m_comment;
			e.compressed_size = file_stat.m_comp_size;
			e.original_size = file_stat.m_uncomp_size;
			e.offset = offset;
			e.is_compressed = is_compressed;
			e.name = file_stat.m_filename;
			return e;
		}
		return std::nullopt;
	}

	std::unique_ptr<archive_reader<zip_entry>> zip_archive::create_reader (zip_entry& entry) {
		if (!entry.is_compressed) {
			return std::make_unique<uncompressed_archive_reader>(m_archive, entry);
		}
		return std::make_unique<compressed_archive_reader>(m_archive, entry);
	}

	zip_archive::~zip_archive () {
		mz_zip_reader_end (&m_archive);
	}


}

