#ifndef VFS_PATH_HH
#define VFS_PATH_HH

#include <vfs/api/vfs_api.h>
#include <string>
#include <vector>
#include <iosfwd>

namespace vfs {
	class path
		/// This class represents filesystem paths in a
		/// platform-independent manner.
		/// Unix, Windows and OpenVMS all use a different
		/// syntax for filesystem paths.
		/// This class can work with all three formats.
		/// A path is made up of an optional node name
		/// (only Windows and OpenVMS), an optional
		/// device name (also only Windows and OpenVMS),
		/// a list of directory names and an optional
		/// filename.
	{
	 public:
		enum style {
			PATH_UNIX,    /// Unix-style path
			PATH_URI = PATH_UNIX, /// URI-style path, same as Unix-style
			PATH_WINDOWS, /// Windows-style path
			PATH_VMS,     /// VMS-style path
			PATH_NATIVE,  /// The current platform's native style
			PATH_GUESS    /// Guess the style by examining the path
		};

		typedef std::vector<std::string> string_vec_t;

		path ();
		/// Creates an empty relative path.

		explicit path (bool absolute);
		/// Creates an empty absolute or relative path.

		path (const char* path);
		/// Creates a path from a string.

		path (const char* path, style style);
		/// Creates a path from a string.

		path (const std::string& path);
		/// Creates a path from a string.

		path (const std::string& path, style style);
		/// Creates a path from a string.

		path (const path& path);
		/// Copy constructor

		path (path&& path) noexcept;
		/// Move constructor.

		path (const path& parent, const std::string& file_name);
		/// Creates a path from a parent path and a filename.
		/// The parent path is expected to reference a directory.

		path (const path& parent, const char* file_name);
		/// Creates a path from a parent path and a filename.
		/// The parent path is expected to reference a directory.

		path (const path& parent, const path& relative);
		/// Creates a path from a parent path and a relative path.
		/// The parent path is expected to reference a directory.
		/// The relative path is appended to the parent path.

		~path ();
		/// Destroys the Path.

		path& operator= (const path& pth);
		/// Assignment operator.

		path& operator= (path&& pth) noexcept;
		/// Move assignment.

		path& operator= (const std::string& pth);
		/// Assigns a string containing a path in native format.

		path& operator= (const char* pth);
		/// Assigns a string containing a path in native format.

		void swap (path& pth);
		/// Swaps the path with another one.

		path& assign (const std::string& pth);
		/// Assigns a string containing a path in native format.

		path& assign (const std::string& pth, style style);
		/// Assigns a string containing a path.

		path& assign (const path& pth);
		/// Assigns the given path.

		path& assign (const char* pth);
		/// Assigns a string containing a path.

		[[nodiscard]] std::string to_string () const;
		/// Returns a string containing the path in native format.

		[[nodiscard]] std::string to_string (style style) const;
		/// Returns a string containing the path in the given format.

		path& parse (const std::string& pth);
		/// Same as assign().

		path& parse (const std::string& pth, style style);
		/// Assigns a string containing a path.

		bool try_parse (const std::string& pth);
		/// Tries to interpret the given string as a path
		/// in native format.
		/// If the path is syntactically valid, assigns the
		/// path and returns true. Otherwise leaves the
		/// object unchanged and returns false.

		bool try_parse (const std::string& pth, style style);
		/// Tries to interpret the given string as a path,
		/// according to the given style.
		/// If the path is syntactically valid, assigns the
		/// path and returns true. Otherwise leaves the
		/// object unchanged and returns false.

		path& parse_directory (const std::string& pth);
		/// The resulting path always refers to a directory and
		/// the filename part is empty.

		path& parse_directory (const std::string& pth, style style);
		/// The resulting path always refers to a directory and
		/// the filename part is empty.

		path& make_directory ();
		/// If the path contains a filename, the filename is appended
		/// to the directory list and cleared. Thus the resulting path
		/// always refers to a directory.

		path& make_file ();
		/// If the path contains no filename, the last directory
		/// becomes the filename.

		path& make_parent ();
		/// Makes the path refer to its parent.

		path& make_absolute (const path& base);
		/// Makes the path absolute if it is relative.
		/// The given path is taken as base.

		path& append (const path& pth);
		/// Appends the given path.

		path& resolve (const path& pth);
		/// Resolves the given path against the current one.
		///
		/// If the given path is absolute, it replaces the current one.
		/// Otherwise, the relative path is appended to the current path.

		[[nodiscard]] bool is_absolute () const;
		/// Returns true iff the path is absolute.

		[[nodiscard]] bool is_relative () const;
		/// Returns true iff the path is relative.

		[[nodiscard]] bool is_directory () const;
		/// Returns true iff the path references a directory
		/// (the filename part is empty).

		[[nodiscard]] bool is_file () const;
		/// Returns true iff the path references a file
		/// (the filename part is not empty).

		path& set_node (const std::string& node);
		/// Sets the node name.
		/// Setting a non-empty node automatically makes
		/// the path an absolute one.

		[[nodiscard]] const std::string& get_node () const;
		/// Returns the node name.

		path& set_device (const std::string& device);
		/// Sets the device name.
		/// Setting a non-empty device automatically makes
		/// the path an absolute one.

		[[nodiscard]] const std::string& get_device () const;
		/// Returns the device name.

		[[nodiscard]] int depth () const;
		/// Returns the number of directories in the directory list.

		[[nodiscard]] const std::string& directory (int n) const;
		/// Returns the n'th directory in the directory list.
		/// If n == depth(), returns the filename.

		const std::string& operator[] (int n) const;
		/// Returns the n'th directory in the directory list.
		/// If n == depth(), returns the filename.

		path& push_directory (const std::string& dir);
		/// Adds a directory to the directory list.

		path& pop_directory ();
		/// Removes the last directory from the directory list.

		path& pop_front_directory ();
		/// Removes the first directory from the directory list.

		path& set_file_name (const std::string& name);
		/// Sets the filename.

		[[nodiscard]] const std::string& get_file_name () const;
		/// Returns the filename.

		path& set_base_name (const std::string& name);
		/// Sets the basename part of the filename and
		/// does not change the extension.

		[[nodiscard]] std::string get_base_name () const;
		/// Returns the basename (the filename sans
		/// extension) of the path.

		path& set_extension (const std::string& extension);
		/// Sets the filename extension.

		[[nodiscard]] std::string get_extension () const;
		/// Returns the filename extension.

		path& clear ();
		/// Clears all components.

		[[nodiscard]] path parent () const;
		/// Returns a path referring to the path's
		/// directory.

		[[nodiscard]] path absolute (const path& base) const;
		/// Returns an absolute variant of the path,
		/// taking the given path as base.

		static path for_directory (const std::string& pth);
		/// Creates a path referring to a directory.

		static path for_directory (const std::string& pth, style style);
		/// Creates a path referring to a directory.

		static char separator ();
		/// Returns the platform's path name separator, which separates
		/// the components (names) in a path.
		///
		/// On Unix systems, this is the slash '/'. On Windows systems,
		/// this is the backslash '\'. On OpenVMS systems, this is the
		/// period '.'.

		[[nodiscard]] size_t hash () const;

	 protected:
		void parse_unix (const std::string& pth);
		void parse_windows (const std::string& pth);
		void parse_vms (const std::string& pth);
		void parse_guess (const std::string& pth);
		[[nodiscard]] std::string build_unix () const;
		[[nodiscard]] std::string build_windows () const;
		[[nodiscard]] std::string build_vms () const;

	 private:
		std::string m_node;
		std::string m_device;
		std::string m_name;
		std::string m_version;
		string_vec_t m_dirs;
		bool m_absolute;
	};

//
// inlines
//
	inline bool path::is_absolute () const {
		return m_absolute;
	}

	inline bool path::is_relative () const {
		return !m_absolute;
	}

	inline bool path::is_directory () const {
		return m_name.empty ();
	}

	inline bool path::is_file () const {
		return !m_name.empty ();
	}

	inline path& path::parse (const std::string& pth) {
		return assign (pth);
	}

	inline path& path::parse (const std::string& pth, style style) {
		return assign (pth, style);
	}

	inline const std::string& path::get_node () const {
		return m_node;
	}

	inline const std::string& path::get_device () const {
		return m_device;
	}

	inline const std::string& path::get_file_name () const {
		return m_name;
	}

	inline int path::depth () const {
		return int (m_dirs.size ());
	}

	inline path path::for_directory (const std::string& pth) {
		path p;
		return p.parse_directory (pth);
	}

	inline path path::for_directory (const std::string& pth, style style) {
		path p;
		return p.parse_directory (pth, style);
	}

	inline char path::separator () {
#if defined(PREDEF_OS_WINDOWS)
		return '\\';
#else
		return '/';
#endif
	}

	inline void swap (path& p1, path& p2) {
		p1.swap (p2);
	}
	// -----------------------------------------------------------------------------

	inline
	std::ostream& operator<< (std::ostream& os, const path& pth) {
		os << pth.to_string();
		return os;
	}
}

#endif
