//
// Created by igor on 2/5/24.
//
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include "miniz.h"
#include <vector>

extern "C" int mz_inflate_no_copy (mz_streamp pStream, int flush);

struct zip_geometry {
	zip_geometry (uint64_t input_file_offset, uint64_t input_chunk_offset, uint64_t input_chunk_size,
				  uint64_t output_offset, uint64_t output_chunk_size)
		: input_file_offset (input_file_offset),
		  input_chunk_offset (input_chunk_offset),
		  input_chunk_size (input_chunk_size),
		  output_offset (output_offset),
		  output_chunk_size (output_chunk_size) {}

	friend std::ostream& operator<< (std::ostream& os, const zip_geometry& geometry) {
		os << "input_file_offset: " << geometry.input_file_offset << " input_chunk_offset: "
		   << geometry.input_chunk_offset << " input_chunk_size: " << geometry.input_chunk_size << " output_offset: "
		   << geometry.output_offset << " output_chunk_size: " << geometry.output_chunk_size;
		return os;
	}

	uint64_t input_file_offset;
	uint64_t input_chunk_offset;
	uint64_t input_chunk_size;
	uint64_t output_offset;
	uint64_t output_chunk_size;
};





class random_access_inflate_reader {
 private:
	static constexpr std::size_t INPUT_BUFFER_SIZE = 64 * 1024;
 public:
	random_access_inflate_reader(FILE* f, std::size_t stride);

	ssize_t read(uint64_t offset, char* out, std::size_t size);
	[[nodiscard]] size_t get_total_size() const;
 private:
	ssize_t read_chunk(mz_stream& stream, uint64_t offset, char* out, std::size_t size);
	size_t read_from_stream(uint64_t offset);
 private:
	FILE* m_file;
	std::size_t m_stride;
	uint64_t    m_offset;
	uint64_t    m_total_out_size;
	uint64_t    m_last_input_chunk_offset;
	size_t      m_last_input_chunk_size;
	unsigned char m_inbuf[INPUT_BUFFER_SIZE];
	std::vector<zip_geometry> m_geometry;
};

random_access_inflate_reader::random_access_inflate_reader(FILE* f, std::size_t stride)
: m_file(f),
  m_stride(stride),
  m_offset (0),
  m_total_out_size (0),
  m_last_input_chunk_size (0) {

	mz_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;

	mz_inflateInit (&stream);

	bool flag = true;
	m_last_input_chunk_offset = 0;
	while (flag) {
		if (!stream.avail_in) {
			m_last_input_chunk_offset = m_offset;
			auto has_bytes = read_from_stream (m_offset);
			m_last_input_chunk_size = has_bytes;
			m_offset += has_bytes;
			stream.next_in = m_inbuf;
			stream.avail_in = has_bytes;
		}
		if (stream.avail_out == 0) {
			stream.avail_out = m_stride;
		}
		const std::size_t output_offset = stream.total_out;
		const uint64_t input_buffer_offset = stream.next_in - m_inbuf;
		int rc = mz_inflate_no_copy (&stream, MZ_SYNC_FLUSH);
		const uint64_t next_input_buffer_offset = stream.next_in - m_inbuf;
		const uint64_t input_chunk_size = next_input_buffer_offset - input_buffer_offset;
		const uint64_t output_chunk_size = stream.total_out - output_offset;

		bool take = false;
		if (rc != 0) {
			if (rc == MZ_STREAM_END) {
				take = true;
			}
			flag = false;
		} else {
			take = true;
		}
		if (take) {
			m_geometry.emplace_back (m_last_input_chunk_offset, input_buffer_offset, input_chunk_size, output_offset, output_chunk_size);
		}
		if (flag == false && rc != MZ_STREAM_END) {
			throw std::runtime_error("Zip stream is corrupted");
		}
	}
	m_total_out_size = stream.total_out;
}

size_t random_access_inflate_reader::read_from_stream (uint64_t offset) {
	fseek (m_file, offset, SEEK_SET);
	return fread (m_inbuf, 1, INPUT_BUFFER_SIZE, m_file);
}

ssize_t random_access_inflate_reader::read (uint64_t offset, char* out, std::size_t size) {
	size_t has_bytes = 0;
	uint64_t write_offset = offset;
	mz_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;

	mz_inflateInit (&stream);

	while (has_bytes < size) {
		size_t to_read = size - has_bytes;
		auto sz = read_chunk (stream, write_offset, out + has_bytes, to_read);
		if (sz < 0) {
			return -1;
		}
		write_offset += sz;
		has_bytes += sz;
	}
	return has_bytes;
}

ssize_t random_access_inflate_reader::read_chunk (mz_stream& stream, uint64_t offset, char* out, std::size_t size) {
	auto itr = std::lower_bound (m_geometry.begin(), m_geometry.end(), offset,
					  [](const auto& g, const auto& o) {
								return g.output_offset < o;
					  });
	if (itr == m_geometry.end()) {
		return -1;
	}
	if (itr->output_offset > offset) {
		itr--;
	}
	if (itr->input_file_offset != m_last_input_chunk_offset) {
		m_last_input_chunk_offset = itr->input_file_offset;
		m_last_input_chunk_size = read_from_stream (m_last_input_chunk_offset);
	}

	stream.next_in  = m_inbuf + itr->input_chunk_offset;
	stream.avail_in = m_last_input_chunk_size - itr->input_chunk_offset;

	auto to_skip = offset - itr->output_offset;
	if (to_skip > 0) {
		stream.avail_out = std::min(to_skip, itr->output_chunk_size);
		int rc = mz_inflate_no_copy(&stream, 0);
		if (rc == MZ_STREAM_END || rc == MZ_OK) {
			return stream.total_out;
		}
	}


	stream.next_out  = (unsigned char*)out + (offset - itr->output_offset);
	stream.avail_out = std::min(size-offset, itr->output_chunk_size);
	int rc = mz_inflate (&stream, MZ_SYNC_FLUSH);
	if (rc == MZ_STREAM_END || rc == MZ_OK) {
		return stream.total_out;
	}
	return -1;
}

size_t random_access_inflate_reader::get_total_size () const {
	return m_total_out_size;
}

int main (int argc, char* argv[]) {
	FILE* f = fopen ("/home/igor/tmp/book.z", "rb");
	if (f == NULL) {
		std::cout << "Failed to open file" << std::endl;
	}

	constexpr std::size_t stride = 1 * 1024;
	random_access_inflate_reader rdr(f, stride);

	size_t has_bytes = 2560;

	while (has_bytes < rdr.get_total_size()) {
		constexpr std::size_t insize = (stride * 3)/2;
		char out[insize];
		auto n = rdr.read (has_bytes, out, insize);
		std::cout << n << std::endl;
		if (n < 0) {
			std::cout << "ZOPA" << std::endl;
		}
		has_bytes += n;
	}

//	mz_stream stream;
//	stream.zalloc = Z_NULL;
//	stream.zfree = Z_NULL;
//	stream.opaque = Z_NULL;
//	stream.avail_in = 0;
//	stream.next_in = Z_NULL;
//
//	mz_inflateInit (&stream);
//
//	unsigned char inbuf[input_buffer_size];
//	std::vector<zip_geometry> data;
//	bool flag = true;
//
//	while (flag) {
//		const uint64_t input_file_offset = ftell (f);
//		if (!stream.avail_in) {
//			auto has_bytes = fread (inbuf, 1, input_buffer_size, f);
//			stream.next_in = inbuf;
//			stream.avail_in = has_bytes;
//		}
//		if (stream.avail_out == 0) {
//			stream.avail_out = stride;
//		}
//		const std::size_t output_offset = stream.total_out;
//		const uint64_t input_buffer_offset = stream.next_in - inbuf;
//		int rc = mz_inflate_no_copy (&stream, MZ_SYNC_FLUSH);
//		const uint64_t next_input_buffer_offset = stream.next_in - inbuf;
//		const uint64_t input_chunk_size = next_input_buffer_offset - input_buffer_offset;
//		const uint64_t output_chunk_size = stream.total_out - output_offset;
//
//		bool take = false;
//		if (rc != 0) {
//			if (rc == MZ_STREAM_END) {
//				take = true;
//			}
//			std::cout << rc << std::endl;
//			flag = false;
//		} else {
//			take = true;
//		}
//		if (take) {
//			data.emplace_back (input_file_offset, input_buffer_offset, input_chunk_size, output_offset, output_chunk_size);
//		}
//	}
//	uint64_t total_size = 0;
//	for (const auto& g : data) {
//		std::cout << g << std::endl;
//		total_size += g.output_chunk_size;
//	}
//	std::cout << total_size << "/" << stream.total_out << std::endl;
	fclose (f);
}

// ====================================================================================================
[[maybe_unused]] static int dump_archive () {
	mz_zip_archive zip_archive = {0};

	auto status = mz_zip_reader_init_file (&zip_archive, "/home/igor/tmp/test.zip", 0);
	if (!status) {
		auto err = mz_zip_get_last_error (&zip_archive);
		std::cout << "Error " << err << std::endl;
		return 1;
	}
	auto num_files = mz_zip_reader_get_num_files (&zip_archive);
	for (int i = 0; i < num_files; i++) {
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat (&zip_archive, i, &file_stat)) {
			printf ("mz_zip_reader_file_stat() failed!\n");
			mz_zip_reader_end (&zip_archive);
			return EXIT_FAILURE;
		}

		printf ("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u, Is Dir: %u\n", file_stat
			.m_filename, file_stat.m_comment, (uint)file_stat.m_uncomp_size, (uint)file_stat
			.m_comp_size, mz_zip_reader_is_file_a_directory (&zip_archive, i));

		if (!strcmp (file_stat.m_filename, "test/256.dat")) {
			size_t pSize;
			mz_zip_reader_extract_file_to_heap (&zip_archive, "test/256.dat", &pSize, 0);
		}
	}

	// Close the archive, freeing any resources it was using
	mz_zip_reader_end (&zip_archive);
	return 0;
}