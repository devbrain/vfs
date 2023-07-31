//
// Created by igor on 7/31/23.
//

#ifndef INCLUDE_VFS_VFS_PATH_HH
#define INCLUDE_VFS_VFS_PATH_HH

#include <string>
#include <vector>
#include <ostream>
#include <vfs/export.h>

namespace vfs {
  class VFS_EXPORT path {
    public:
      explicit path(std::string a_path);

      path(const path& ) = default;
      path& operator = (const path& ) = default;

      [[nodiscard]] bool operator == (const path& other) const;
      [[nodiscard]] bool operator != (const path& other) const;
      [[nodiscard]] bool operator < (const path& other) const;
      [[nodiscard]] bool operator <= (const path& other) const;
      [[nodiscard]] bool operator > (const path& other) const;
      [[nodiscard]] bool operator >= (const path& other) const;

      [[nodiscard]] const std::string& string() const;
      [[nodiscard]] const char* c_str() const;

      // Get dirname part of a path.
      // Depending on the path, an empty string may be returned.
      // Example: "/path/to/foo/bar.txt" returns "/path/to/foo"
      [[nodiscard]] std::string dirname () const;

      // Get filename part of a path.
      // Depending on the path, an empty string may be returned.
      // Example: "/path/to/foo/bar.txt" returns "bar.txt"

      [[nodiscard]] std::string filename () const;

      // Get filename part of a path, without extension.
      // Depending on the path, an empty string may be returned.
      // Example: "/path/to/foo/bar.txt" returns "bar"

      [[nodiscard]] std::string filebase () const;


      // Split the pathname path into a pair (root, ext) such that root + ext == path,
      // and ext is empty or begins with a period and contains at most one period.
      // Leading periods on the basename are ignored; splitext(".cshrc") returns {".cshrc", ""}.

      [[nodiscard]] std::vector<std::string> splitext () const;

      // Get the extension of a path.
      // Depending on the path, an empty string may be returned.
      // Example: "/path/to/foo/bar.txt" returns "txt"

      [[nodiscard]]  std::string ext () const;


      // Split sub-paths using the separator.
      // Option: slice the output[begin: end]

      [[nodiscard]] std::vector<std::string> split () const;


      [[nodiscard]] static path normalize (const path& pth);

      // Select the common prefix in a list of strings.
      [[nodiscard]] std::string common_prefix (const path& other) const;
      [[nodiscard]] static std::string common_prefix (const std::vector<std::string>& paths);
      [[nodiscard]] static std::string common_prefix (const std::vector<path>& paths);

      // Select the common dirname in a list of paths.
      [[nodiscard]] std::string common_dirname (const path& other) const;
      [[nodiscard]] static std::string common_dirname (const std::vector<std::string>& paths);
      [[nodiscard]] static std::string common_dirname (const std::vector<path>& paths);
    private:
      std::string m_path;
  };

  VFS_EXPORT std::ostream& operator << (std::ostream& os, const path& p);
}

#endif //INCLUDE_VFS_VFS_PATH_HH
