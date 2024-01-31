//
// Created by igor on 1/31/24.
//

#include <vfs/stream.hh>
#include <vfs/io.hh>
#include <bsw/exception.hh>
#include <bsw/io/ios_init.hh>

namespace vfs {
	file_stream_buf::file_stream_buf ()
		:
		bsw::io::buffered_bidi_stream_buf (BUFFER_SIZE, std::ios::in | std::ios::out),
		_fd (nullptr),
		_pos (0) {
	}

	file_stream_buf::~file_stream_buf () {
		close ();
	}

	void file_stream_buf::open (const std::string& path, std::ios::openmode m) {
		ENFORCE(!_fd);

		_pos = 0;
		_path = path;
		mode (m);
		reset_buffers ();

		creation_disposition flags;
		bool read_only = false;

		if (m & std::ios::in) {
			read_only = true;
			flags = creation_disposition::eOPEN_EXISTING;
			if (m & std::ios::out) {
				read_only = false;
				flags = creation_disposition::eCREATE_ALWAYS;
			}
		}

		if (m & std::ios::trunc) {
			read_only = false;
			flags = creation_disposition::eTRUNCATE_EXISTING;
		}
		if (m & std::ios::app) {
			read_only = false;
			flags = creation_disposition::eOPEN_EXISTING;
		}

		_fd = vfs::open (path, flags, read_only);

		if ((m & std::ios::app) || (m & std::ios::ate)) {
			seekoff (0, std::ios::end, m);
		}
	}

	int file_stream_buf::read_from_device (char* buffer, std::streamsize length) {
		if (!_fd) { return -1; }

		if (mode () & std::ios::out) {
			sync ();
		}

		auto n = read (_fd, buffer, length);
		_pos += n;
		return n;
	}

	int file_stream_buf::write_to_device (const char* buffer, std::streamsize length) {
		if (!_fd) { return -1; }

		auto n = write (_fd, buffer, length);

		_pos += n;
		return n;
	}

	bool file_stream_buf::close () {
		bool success = true;
		if (_fd) {
			try {
				sync ();
			}
			catch (...) {
				success = false;
			}
			::vfs::close (_fd);
			_fd = nullptr;
		}
		return success;
	}

	std::streampos file_stream_buf::seekoff (std::streamoff off, std::ios::seekdir dir, std::ios::openmode m) {
		if (!_fd || !(mode () & m)) {
			return -1;
		}

		if (mode () & std::ios::out) {
			sync ();
		}

		std::streamoff adj;
		if (m & std::ios::in) {
			adj = static_cast<std::streamoff>(egptr () - gptr ());
		} else {
			adj = 0;
		}

		reset_buffers ();

		seek_type whence = seek_type::eSET;
		if (dir == std::ios::cur) {
			whence = seek_type::eCUR;
			off -= adj;
		} else if (dir == std::ios::end) {
			whence = seek_type::eEND;
		}
		::vfs::seek (_fd, off, whence);
		_pos = ::vfs::tell (_fd);
		return _pos;
	}

	std::streampos file_stream_buf::seekpos (std::streampos pos, std::ios::openmode m) {
		if (!_fd || !(mode () & m)) {
			return -1;
		}

		if (mode () & std::ios::out) {
			sync ();
		}

		reset_buffers ();
		vfs::seek (_fd, pos, seek_type::eSET);
		_pos = vfs::tell (_fd);
		return _pos;
	}

	// ==================================================================================================
	file_ios::file_ios (std::ios::openmode default_mode)
		:
		_defaultMode (default_mode) {
		bsw_ios_init(&_buf);
	}

	file_ios::~file_ios () = default;

	void file_ios::open (const std::string& path, std::ios::openmode mode) {
		clear ();
		_buf.open (path, mode | _defaultMode);
	}

	void file_ios::close () {
		if (!_buf.close ()) {
			setstate (ios_base::badbit);
		}
	}

	file_stream_buf* file_ios::rdbuf () {
		return &_buf;
	}

	file_input_stream::file_input_stream ()
		:
		file_ios (std::ios::in),
		std::istream (&_buf) {
	}

	file_input_stream::file_input_stream (const std::string& path, std::ios::openmode mode)
		:
		file_ios (std::ios::in),
		std::istream (&_buf) {
		open (path, mode);
	}

	file_input_stream::~file_input_stream () = default;

	file_output_stream::file_output_stream ()
		:
		file_ios (std::ios::out),
		std::ostream (&_buf) {
	}

	file_output_stream::file_output_stream (const std::string& path, std::ios::openmode mode)
		:
		file_ios (std::ios::out),
		std::ostream (&_buf) {
		open (path, mode);
	}

	file_output_stream::~file_output_stream () = default;

	file_stream::file_stream ()
		:
		file_ios (std::ios::in | std::ios::out),
		std::iostream (&_buf) {
	}

	file_stream::file_stream (const std::string& path, std::ios::openmode mode)
		:
		file_ios (std::ios::in | std::ios::out),
		std::iostream (&_buf) {
		open (path, mode);
	}

	file_stream::~file_stream () = default;

}