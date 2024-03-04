//
// Created by igor on 3/3/24.
//

#include <algorithm>
#include <bsw/exception.hh>
#include "zipfs_stream_file.hh"


namespace vfs::extra {

	zipfs_stream_file::zipfs_stream_file (zip_archive* archive, const zip_tree* node)
		: zipfs_uncompressed_file (archive, node),
		m_infile_remaining(node->compressed_size),
		m_next_out(nullptr),
		m_avail_out(0),
		m_total_out(0),
		m_next_in(nullptr),
		m_avail_in(0),
		m_total_in(0),
		m_outbuf_len(0),
		m_outbuf_ptr(0) {
		tinfl_init(&m_inflator);
		m_next_out = m_outbuf;
		m_avail_out = OUTPUT_BUFFER_SIZE;

	}

	ssize_t zipfs_stream_file::read_chunk() {
		size_t in_bytes, out_bytes;
		tinfl_status status;
		if (!m_avail_in)
		{
			// Input buffer is empty, so read more bytes from input file.
			auto to_read = std::min(INPUT_BUFFER_SIZE, m_infile_remaining);
			auto n = m_archive->read (m_inbuf, to_read, m_entry_offset + m_total_in);
			m_next_in = m_inbuf;
			m_avail_in = n;
			m_infile_remaining -= n;
		}
		in_bytes = m_avail_in;
		out_bytes = m_avail_out;
		status = tinfl_decompress(&m_inflator, (const mz_uint8 *)m_next_in, &in_bytes, m_outbuf, m_next_out, &out_bytes, (m_infile_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0));

		m_avail_in -= in_bytes;
		m_next_in = m_next_in + in_bytes;
		m_total_in += in_bytes;

		m_avail_out -= out_bytes;
		m_next_out = m_next_out + out_bytes;
		m_total_out += out_bytes;

		std::size_t out_buff_size = 0;
		if ((status <= TINFL_STATUS_DONE) || (!m_avail_out))
		{
			// Output buffer is full, or decompression is done, so write buffer to output file.
			out_buff_size = OUTPUT_BUFFER_SIZE - m_avail_out;
			m_next_out = m_outbuf;
			m_avail_out = OUTPUT_BUFFER_SIZE;

		}

		// If status is <= TINFL_STATUS_DONE then either decompression is done or something went wrong.
		if (status <= TINFL_STATUS_DONE) {
			if (status == TINFL_STATUS_DONE) {
				// Decompression completed successfully.
				return out_buff_size;
			} else {
				return -1;
			}
		}
		return out_buff_size;
	}


	ssize_t zipfs_stream_file::read (void* buff, size_t len) {
		if (m_pointer >= m_file_size) {
			return 0;
		}

		auto bytes_in_obuf = m_outbuf_len - m_outbuf_ptr;
		if (bytes_in_obuf < len) {
			auto rc = read_chunk();
			if (rc < 0) {
				return rc;
			}
			bytes_in_obuf = (std::size_t)rc;
			m_outbuf_ptr = 0;
		}
		auto can_read = std::min(bytes_in_obuf, len);
		std::memcpy (buff, m_outbuf + m_outbuf_ptr, can_read);
		m_outbuf_ptr += can_read;
		m_pointer += can_read;
	}

	bool zipfs_stream_file::seek (uint64_t pos, whence_type whence) {
		RAISE_EX("SEEK operation is not supported on ZIPFS");
	}

	zipfs_stream_file::~zipfs_stream_file () {
		tinfl_decompressor_free(&m_inflator);
	}
}
