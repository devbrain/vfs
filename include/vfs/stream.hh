//
// Created by igor on 1/31/24.
//

#ifndef VFS_INCLUDE_VFS_STREAM_HH_
#define VFS_INCLUDE_VFS_STREAM_HH_

#include <streambuf>
#include <iosfwd>
#include <ios>
#include <istream>
#include <ostream>

#include <bsw/io/buffered_bidi_stream_buf.hh>
#include <vfs/api/vfs_api.h>

#include <bsw/warn/push.hh>
#include <bsw/warn/dll_interface>

namespace vfs {

	struct file;

	class VFS_API file_stream_buf : public bsw::io::buffered_bidi_stream_buf
		/// This stream buffer handles Fileio
	{
	 public:
		file_stream_buf ();
		/// Creates a FileStreamBuf.

		~file_stream_buf () override;
		/// Destroys the FileStream.

		void open (const std::string& path, std::ios::openmode mode);
		/// Opens the given file in the given mode.

		bool close ();
		/// Closes the File stream buffer. Returns true if successful,
		/// false otherwise.

		std::streampos
		seekoff (std::streamoff off, std::ios::seekdir dir, std::ios::openmode mode = std::ios::in
																					  | std::ios::out) override;
		/// Change position by offset, according to way and mode.

		std::streampos seekpos (std::streampos pos, std::ios::openmode mode = std::ios::in | std::ios::out) override;
		/// Change to specified position, according to mode.

	 protected:
		enum {
			BUFFER_SIZE = 4096
		};

		int read_from_device (char* buffer, std::streamsize length) override;
		int write_to_device (const char* buffer, std::streamsize length) override;

	 private:
		std::string _path;
		file* _fd;
		std::streamoff _pos;
	};

	class VFS_API file_ios : public virtual std::ios
		/// The base class for FileInputStream and FileOutputStream.
		///
		/// This class is needed to ensure the correct initialization
		/// order of the stream buffer and base classes.
		///
		/// Files are always opened in binary mode, a text mode
		/// with CR-LF translation is not supported. Thus, the
		/// file is always opened as if the std::ios::binary flag
		/// was specified.
		/// Use an InputLineEndingConverter or OutputLineEndingConverter
		/// if you require CR-LF translation.
		///
		/// On Windows platforms, UTF-8 encoded Unicode paths are correctly handled.
	{
	 public:
		explicit file_ios (std::ios::openmode default_mode);
		/// Creates the basic stream.

		~file_ios () override;
		/// Destroys the stream.

		void open (const std::string& path, std::ios::openmode mode);
		/// Opens the file specified by path, using the given mode.
		///
		/// Throws a FileException (or a similar exception) if the file
		/// does not exist or is not accessible for other reasons and
		/// a new file cannot be created.

		void close ();
		/// Closes the file stream.
		///
		/// If, for an output stream, the close operation fails (because
		/// the contents of the stream buffer cannot synced back to
		/// the filesystem), the bad bit is set in the stream state.

		file_stream_buf* rdbuf ();
		/// Returns a pointer to the underlying streambuf.

	 protected:
		file_stream_buf _buf;
		std::ios::openmode _defaultMode;
	};

	class VFS_API file_input_stream : public file_ios, public std::istream
/// An input stream for reading from a file.
///
/// Files are always opened in binary mode, a text mode
/// with CR-LF translation is not supported. Thus, the
/// file is always opened as if the std::ios::binary flag
/// was specified.
/// Use an InputLineEndingConverter if you require CR-LF translation.
///
/// On Windows platforms, UTF-8 encoded Unicode paths are correctly handled.
	{
	 public:
		file_input_stream ();
/// Creates an unopened FileInputStream.

		explicit file_input_stream (const std::string& path, std::ios::openmode mode = std::ios::in);
/// Creates the FileInputStream for the file given by path, using
/// the given mode.
///
/// The std::ios::in flag is always set, regardless of the actual
/// value specified for mode.
///
/// Throws a FileNotFoundException (or a similar exception) if the file
/// does not exist or is not accessible for other reasons.

		~file_input_stream () override;
/// Destroys the stream.
	};

	class VFS_API file_output_stream : public file_ios, public std::ostream
/// An output stream for writing to a file.
///
/// Files are always opened in binary mode, a text mode
/// with CR-LF translation is not supported. Thus, the
/// file is always opened as if the std::ios::binary flag
/// was specified.
/// Use an OutputLineEndingConverter if you require CR-LF translation.
///
/// On Windows platforms, UTF-8 encoded Unicode paths are correctly handled.
	{
	 public:
		file_output_stream ();
/// Creats an unopened FileOutputStream.

		explicit file_output_stream (const std::string& path, std::ios::openmode mode = std::ios::out | std::ios::trunc);
/// Creates the FileOutputStream for the file given by path, using
/// the given mode.
///
/// The std::ios::out is always set, regardless of the actual
/// value specified for mode.
///
/// Throws a FileException (or a similar exception) if the file
/// does not exist or is not accessible for other reasons and
/// a new file cannot be created.

		~file_output_stream () override;
/// Destroys the FileOutputStream.
	};

	class VFS_API file_stream : public file_ios, public std::iostream
/// A stream for reading from and writing to a file.
///
/// Files are always opened in binary mode, a text mode
/// with CR-LF translation is not supported. Thus, the
/// file is always opened as if the std::ios::binary flag
/// was specified.
/// Use an InputLineEndingConverter or OutputLineEndingConverter
/// if you require CR-LF translation.
///
/// A seek (seekg() or seekp()) operation will always set the
/// read position and the write position simultaneously to the
/// same value.
///
/// On Windows platforms, UTF-8 encoded Unicode paths are correctly handled.
	{
	 public:
		file_stream ();
/// Creats an unopened FileStream.

		explicit file_stream (const std::string& path, std::ios::openmode mode = std::ios::out | std::ios::in);
/// Creates the FileStream for the file given by path, using
/// the given mode.

		~file_stream () override;
/// Destroys the FileOutputStream.
	};

} // ns vfs

#include <bsw/warn/pop.hh>

#endif //VFS_INCLUDE_VFS_STREAM_HH_
