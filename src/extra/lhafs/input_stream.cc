//
// Created by igor on 3/19/24.
//

#include <istream>
#include "input_stream.hh"


namespace {
	/**
	 * Read a block of data into the specified buffer.
	 *
	 * @param handle       Handle pointer.
	 * @param buf          Pointer to buffer in which to store read data.
	 * @param buf_len      Size of buffer, in bytes.
	 * @return             Number of bytes read, or -1 for error.
	 */

	int read(void *handle, void *buf, size_t buf_len) {
		auto* is = reinterpret_cast<std::istream*>(handle);
		auto pos = is->tellg();
		if (!is->read ((char*)buf, buf_len)) {
			return -1;
		}
		auto curr = is->tellg();
		return (size_t)curr - (size_t)pos;
	}


	/**
	 * Skip the specified number of bytes from the input stream.
	 * This is an optional function.
	 *
	 * @param handle       Handle pointer.
	 * @param bytes        Number of bytes to skip.
	 * @return             Non-zero for success, or zero for failure.
	 */

	int skip (void *handle, size_t bytes) {
		auto* is = reinterpret_cast<std::istream*>(handle);
		if (!is->seekg (bytes, std::ios::cur)) {
			return 0;
		}
		return 1;
	}

	/**
	 * Close the input stream.
	 *
	 * @param handle       Handle pointer.
	 */

	void close([[maybe_unused]] void *handle) {

	}
}

namespace vfs::extra {
	LHAInputStreamType create_lha_istream() {
		return {read, skip, close};
	}
}