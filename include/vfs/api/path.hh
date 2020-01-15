#ifndef VFS_PATH_HH
#define VFS_PATH_HH

#include <vfs/api/vfs_api.h>
#include <string>
#include <vector>
#include <iosfwd>

namespace vfs
{
	class VFS_API path
	{
	public:
		typedef std::vector<std::string> string_vec_t;
		/**
		 * Creates an empty relative path.
		 */
		path();

		/**
		 * Creates a path from a string.
		 */
		path(const char* path);

		/**
		 * Creates a path from a string.
		 */
		path(const std::string& path);

		/**
		 * Copy constructor
		 */
		path(const path& path);

		/**
		 * Creates a path from a parent path and a filename.
		 * The parent path is expected to reference a directory.
		 */
		path(const path& parent, const std::string& fileName);

		/**
		 * Creates a path from a parent path and a filename.
		 * The parent path is expected to reference a directory.
		 */
		path(const path& parent, const char* fileName);

		/**
		 * Destroys the path.
		 */
		~path();

		/**
		 * Creates a path from a parent path and a filename.
		 * The parent path is expected to reference a directory.
		 */
		path& operator=(const path& path);

		/**
		 * Assigns a string containing a path in native format.
		 */
		path& operator=(const std::string& path);

		/**
		 * Assigns a string containing a path in native format.
		 */

		path& operator=(const char* path);

		/**
		 * Swaps the path with another one.
		 */
		void swap(path& path);

		/**
		 * Assigns a string containing a path in native format.
		 */
		path& assign(const std::string& path);

		/**
		 * Assigns the given path.
		 */
		path& assign(const path& path);
		/**
		 * Assigns a string containing a path.
		 */

		path& assign(const char* path);
		/**
		 * Returns a string containing the path in native format.
		 */
		[[nodiscard]] std::string to_string() const;

		/**
		 * Same as assign().
		 */
		path& parse(const std::string& path);

		/**
		 * Tries to interpret the given string as a path
		 * in native format.
		 * If the path is syntactically valid, assigns the
		 * path and returns true. Otherwise leaves the
		 * object unchanged and returns false.
		 */
		bool try_parse(const std::string& path);

		/**
		 * The resulting path always refers to a directory and
		 * the filename part is empty.
		 */
		path& parse_directory(const std::string& path);

		/**
		 * If the path contains a filename, the filename is appended
		 * to the directory list and cleared. Thus the resulting path
		 * always refers to a directory.
		 */
		path& make_directory();

		/**
		 * If the path contains no filename, the last directory
		 * becomes the filename.
		 */
		path& make_file();

		/**
		 * Makes the path refer to its parent.
		 */
		path& make_parent();

		/**
		 * Appends the given path.
		 */
		path& append(const path& path);

		/**
		 * Returns true iff the path references a directory
		 * (the filename part is empty).
		 */
		[[nodiscard]] bool is_directory() const;

		/**
		 * Returns true iff the path references a file
		 * (the filename part is not empty).
		 */
		[[nodiscard]] bool is_file() const;

		[[nodiscard]] bool empty() const;

		/**
		 * Returns the number of directories in the directory list.
		 */
		[[nodiscard]] int depth() const;

		/**
		 * Returns the n'th directory in the directory list.
		 * If n == depth(), returns the filename.
		 */
		[[nodiscard]] const std::string& directory(int n) const;

		/**
		 * Returns the n'th directory in the directory list.
		 * If n == depth(), returns the filename.
		 */
		[[nodiscard]] const std::string& operator[](int n) const;

		/**
		 * Adds a directory to the directory list.
		 */
		void push_directory(const std::string& dir);

		/**
		 * Removes the last directory from the directory list.
		 */
		void pop_directory();

		/**
		 * Sets the filename.
		 */
		void set_file_name(const std::string& name);

		/**
		 * Returns the filename.
		 */
		[[nodiscard]] const std::string& get_file_name() const;

		/**
		 * Sets the basename part of the filename and
		 * does not change the extension.
		 */
		void set_base_name(const std::string& name);

		/**
		 * Returns the basename (the filename sans
		 * extension) of the path.
		 */
		[[nodiscard]] std::string get_base_name() const;

		/**
		 * Sets the filename extension.
		 */
		void set_extension(const std::string& extension);

		/**
		 * Returns the filename extension.
		 */
		[[nodiscard]] std::string get_extension() const;

		/**
		 * Clears all components.
		 */
		void clear();

		/**
		 * Returns a path referring to the path's
		 * directory.
		 */
		[[nodiscard]] path parent() const;

		/**
		 * Creates a path referring to a directory.
		 */
		[[nodiscard]] static path for_directory(const std::string& path);


	protected:
		void _parse_unix(const std::string& path);

		[[nodiscard]] std::string _build_unix() const;
	private:
#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable:4251)
#endif
		std::string  _name;
		string_vec_t _dirs;

#if defined(_MSC_VER)
#pragma warning (pop)
#endif
	};
	// -----------------------------------------------------------------------------
	VFS_API void swap(path& p1, path& p2);
	[[nodiscard]] VFS_API std::string basename(const path& pth);
	[[nodiscard]] VFS_API path dirname(const path& pth);
	VFS_API std::ostream& operator<<(std::ostream& os, const path& pth);
}

#endif