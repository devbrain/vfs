#ifndef VFS_PATH_HH
#define VFS_PATH_HH

#include <vfs/api/vfs_api.h>
#include <string>
#include <vector>
#include <iosfwd>

namespace vfs
{
	class path
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
		explicit path(const std::string& path);
        path(const path& path) = default;

		path& operator=(const path& path) = delete;

		/**
		 * Returns a string containing the path in native format.
		 */
		[[nodiscard]] std::string to_string() const;


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
		 * Returns the number of directories in the directory list.
		 */
		[[nodiscard]] int depth() const;


		/**
		 * Returns the n'th directory in the directory list.
		 * If n == depth(), returns the filename.
		 */
		[[nodiscard]] const std::string& operator[](int n) const;


		/**
		 * Returns the filename.
		 */
		[[nodiscard]] const std::string& get_file_name() const;


        size_t hash () const;

	protected:
        /**
         * Adds a directory to the directory list.
         */
        void push_directory(const std::string& dir);

        /**
		 * Clears all components.
		 */
        void clear();

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

	std::ostream& operator<<(std::ostream& os, const path& pth);
}

#endif
