//
// Created by igor on 2/18/24.
//

#include <istream>
#include <algorithm>
#include <bsw/exception.hh>
#include <bsw/strings/string_tokenizer.hh>
#include <bsw/logger/logger.hh>


#include "zip_archive.hh"



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
		default:
			RAISE_EX("Unknown error");
	}
}

namespace vfs::extra {
	zip_archive::zip_archive (std::unique_ptr<std::istream> istream, uint64_t file_size)
		: m_istream (std::move (istream)),
		  m_archive{},
		  m_root(new zip_tree) {
		m_root->offset = file_size;
		mz_zip_zero_struct(&m_archive);
		m_archive.m_pIO_opaque = m_istream.get();
		m_archive.m_pRead = file_read_func;
		auto rc = mz_zip_reader_init (&m_archive, file_size, 0);
		if (rc == MZ_FALSE) {
			raise_last_error (m_archive);
		}
		auto num_files = mz_zip_reader_get_num_files(&m_archive);
		for (unsigned int i = 0; i < num_files; i++) {
			mz_zip_archive_file_stat file_stat;
			if (!mz_zip_reader_file_stat (&m_archive, i, &file_stat)) {
				raise_last_error (m_archive);
			}
			add_to_tree (file_stat);
		}
	}

	zip_archive::~zip_archive () {
		mz_zip_reader_end (&m_archive);
		delete m_root;
	}

	void zip_archive::add_to_tree (const mz_zip_archive_file_stat& s) {
		constexpr uint16_t MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED = 1;
		constexpr uint16_t MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_COMPRESSED_PATCH_FLAG = 32;
		constexpr uint16_t MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION = 64;

		if (s.m_bit_flag & (MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_USES_STRONG_ENCRYPTION | MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_COMPRESSED_PATCH_FLAG)) {
			EVLOG_TRACE(EVLOG_WARNING, "Encrypted files are unsupported");
			return;
		}
		uint64_t offset = m_archive.m_archive_size;
		bool is_compressed = false;
		if (!s.m_is_directory) {
			offset = get_data_offset (s);
			is_compressed = s.m_uncomp_size != s.m_comp_size;
			auto input_size = is_compressed ? s.m_comp_size : s.m_uncomp_size;
			if (offset + input_size > m_archive.m_archive_size) {
				EVLOG_TRACE(EVLOG_WARNING, "Corrupted header for zip file entry at index ", s.m_file_index);
				return;
			}
		}

		zip_tree* current = m_root;

		bsw::string_tokenizer tokenizer(s.m_filename, "/\\",
										bsw::string_tokenizer::TOK_IGNORE_EMPTY | bsw::string_tokenizer::TOK_TRIM);
		for (const auto & name : tokenizer) {
			auto i = current->children.find (name);
			if (i == current->children.end()) {
				auto* node = new zip_tree;
				current->children[name] = node;
				current = node;
			} else {
				current = i->second;
			}
		}
		current->is_dir = s.m_is_directory;
		current->comments = s.m_comment;
		current->compressed_size = s.m_comp_size;
		current->original_size = s.m_uncomp_size;
		current->offset = offset;
		current->is_compressed = is_compressed;
	}

	zip_tree* zip_archive::get_root () {
		return m_root;
	}

	uint64_t zip_archive::get_data_offset(const mz_zip_archive_file_stat& s) const {
		constexpr uint16_t MZ_ZIP_LOCAL_DIR_HEADER_SIZE = 30;
		mz_uint32 local_header_u32[(MZ_ZIP_LOCAL_DIR_HEADER_SIZE + sizeof(mz_uint32) - 1) / sizeof(mz_uint32)];
		auto *pLocal_header = (mz_uint8 *)local_header_u32;
		auto cur_file_ofs = s.m_local_header_ofs;
		if (m_archive.m_pRead(m_archive.m_pIO_opaque, cur_file_ofs, pLocal_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE) {
			RAISE_EX("Failed to read zip file header for index ", s.m_file_index);
		}

		constexpr uint32_t MZ_ZIP_LOCAL_DIR_HEADER_SIG = 0x04034b50;

		if (MZ_READ_LE32(pLocal_header) != MZ_ZIP_LOCAL_DIR_HEADER_SIG) {
			RAISE_EX("Zip header is corrupted for entry at index ", s.m_file_index);
		}
		constexpr uint32_t MZ_ZIP_LDH_FILENAME_LEN_OFS = 26;
		constexpr uint32_t MZ_ZIP_LDH_EXTRA_LEN_OFS = 28;
		cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_FILENAME_LEN_OFS) + MZ_READ_LE16(pLocal_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
		return cur_file_ofs;
	}

	size_t zip_archive::read (void* buf, size_t len, uint64_t offset) const {
		return m_archive.m_pRead(m_archive.m_pIO_opaque, offset, buf, len);
	}

	uint64_t zip_archive::decompress (std::vector<char>& out, uint64_t entry_offset, uint64_t entry_size) const {
		// Decompression.
		auto infile_remaining = entry_size;
		void *next_out = out.data();
		uint64_t avail_out = out.size();

		size_t total_in = 0, total_out = 0;
		size_t avail_in = 0;

		constexpr uint64_t IN_BUF_SIZE = 1024*512;
		static uint8_t s_inbuf[IN_BUF_SIZE];
		uint8_t* next_in = s_inbuf;
		uint64_t offset = entry_offset;

		tinfl_decompressor inflator;
		tinfl_init(&inflator);

		for ( ; ; )
		{
			size_t in_bytes, out_bytes;
			tinfl_status status;
			if (!avail_in)
			{
				// Input buffer is empty, so read more bytes from input file.
				uint n = std::min(IN_BUF_SIZE, infile_remaining);

				if (read(s_inbuf, n, offset) != n) {
					RAISE_EX("zipfs I/O error");
				}
				offset += n;

				next_in = s_inbuf;
				avail_in = n;

				infile_remaining -= n;
			}

			in_bytes = avail_in;
			out_bytes = avail_out;

			status = tinfl_decompress(&inflator,
									  next_in,
									  &in_bytes,
									  (mz_uint8 *)out.data(),
									  (mz_uint8 *)next_out,
									  &out_bytes,
									  (infile_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0));

			avail_in -= in_bytes;
			next_in = next_in + in_bytes;
			total_in += in_bytes;

			avail_out -= out_bytes;
			next_out = (mz_uint8 *)next_out + out_bytes;
			total_out += out_bytes;

			// If status is <= TINFL_STATUS_DONE then either decompression is done or something went wrong.
			if (status <= TINFL_STATUS_DONE)
			{
				if (status == TINFL_STATUS_DONE)
				{
					// Decompression completed successfully.
					break;
				}
				else
				{
					switch (status) {
						case TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS:
							RAISE_EX("zipfs decompression failure: CANNOT_MAKE_PROGRESS");
						case TINFL_STATUS_BAD_PARAM:
							RAISE_EX("zipfs decompression failure: BAD_PARAM");
						case TINFL_STATUS_ADLER32_MISMATCH:
							RAISE_EX("zipfs decompression failure: ADLER32_MISMATCH");
						case TINFL_STATUS_FAILED:
							RAISE_EX("zipfs decompression failure: STATUS_FAILED");
						default:
							RAISE_EX("zipfs decompression failure: Should not be here");
					}
					// Decompression failed.
				}
			}
		}
		return total_out;
	}
	// ===========================================================================================
	zip_tree::zip_tree()
	: is_dir(true),
	  is_compressed(false),
	  original_size(0),
	  compressed_size(0),
	  offset(0) {}

	zip_tree::~zip_tree() {
		for (auto [k, v] : children) {
			delete v;
		}
	}
}

