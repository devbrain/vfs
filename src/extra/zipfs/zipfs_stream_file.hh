//
// Created by igor on 3/3/24.
//

#ifndef VFS_SRC_EXTRA_ZIPFS_ZIPFS_STREAM_FILE_HH_
#define VFS_SRC_EXTRA_ZIPFS_ZIPFS_STREAM_FILE_HH_

#include "zipfs_uncompressed_file.hh"
#include "miniz.h"


namespace vfs::extra {
	class zipfs_stream_file : public zipfs_uncompressed_file {
	 public:
		static constexpr std::size_t INPUT_BUFFER_SIZE = 64 * 1024;
		static constexpr std::size_t OUTPUT_BUFFER_SIZE = 64 * 1024;
	 public:
		zipfs_stream_file (zip_archive* archive, const zip_tree* node);
		~zipfs_stream_file() override;
	 private:
		ssize_t read (void* buff, size_t len) override;
		ssize_t read_chunk();
		bool seek ([[maybe_unused]] uint64_t pos, [[maybe_unused]] whence_type whence) override;
	 private:
		uint64_t m_infile_remaining;

		mz_uint8* m_next_out;
		uint64_t  m_avail_out;
		size_t    m_total_out;

		mz_uint8*  m_next_in;
		size_t     m_avail_in;
		size_t     m_total_in;
		tinfl_decompressor m_inflator;
		unsigned char m_inbuf[INPUT_BUFFER_SIZE];
		unsigned char m_outbuf[OUTPUT_BUFFER_SIZE];

		std::size_t m_outbuf_len;
		std::size_t m_outbuf_ptr;

	};
}

#endif //VFS_SRC_EXTRA_ZIPFS_ZIPFS_STREAM_FILE_HH_
