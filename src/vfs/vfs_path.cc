//
// Created by igor on 7/31/23.
//

#include <bsw/exception.hh>
#include <vfs/vfs_path.hh>
#include "path.hh"

namespace vfs {
  path::path (std::string a_path)
      : m_path (std::move (a_path)) {
    if (m_path.empty()) {
      RAISE_EX("Path can not be empty");
    }
    if (m_path[0] != CPPPATH_SEP[0]) {
      RAISE_EX("Path should be absolute");
    }
  }

  bool path::operator == (const path& other) const {
    return m_path == other.m_path;
  }

  bool path::operator != (const path& other) const {
    return m_path != other.m_path;
  }

  bool path::operator < (const path& other) const {
    return m_path < other.m_path;
  }

  bool path::operator <= (const path& other) const {
    return m_path <= other.m_path;
  }

  bool path::operator > (const path& other) const {
    return m_path > other.m_path;
  }

  bool path::operator >= (const path& other) const {
    return m_path >= other.m_path;
  }

  const std::string& path::string () const {
    return m_path;
  }

  const char* path::c_str () const {
    return m_path.c_str ();
  }

  std::string path::dirname () const {
    return detail::path::dirname (m_path);
  }

  std::string path::filename () const {
    return detail::path::filename (m_path);
  }

  std::string path::filebase () const {
    return detail::path::filebase (m_path);
  }

  std::vector<std::string> path::splitext () const {
    return detail::path::split (m_path);
  }

  std::string path::ext () const {
    return detail::path::ext (m_path);
  }

  std::vector<std::string> path::split () const {
    return detail::path::split (m_path);
  }

  path path::normalize (const path& pth) {
    return path{detail::path::normpath (pth.m_path)};
  }

  std::string path::common_prefix (const path& other) const {
    std::vector<path> dt{*this, other};
    return common_prefix (dt);
  }

  std::string path::common_prefix (const std::vector<std::string>& paths) {
    return detail::path::commonprefix (paths);
  }

  std::string path::common_prefix (const std::vector<path>& paths) {
    std::vector<std::string> dt;
    for (const auto& p: paths) {
      dt.emplace_back (p.m_path);
    }
    return detail::path::commonprefix (dt);
  }

  std::string path::common_dirname (const path& other) const {
    std::vector<path> dt{*this, other};
    return common_dirname (dt);
  }

  std::string path::common_dirname (const std::vector<std::string>& paths) {
    return detail::path::commondirname (paths);
  }

  std::string path::common_dirname (const std::vector<path>& paths) {
    std::vector<std::string> dt;
    for (const auto& p: paths) {
      dt.emplace_back (p.m_path);
    }
    return detail::path::commondirname (dt);
  }

  std::ostream& operator << (std::ostream& os, const path& p) {
    os << p.string();
    return os;
  }
}
