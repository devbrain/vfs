//
// Created by igor on 7/30/23.
//
#include <cctype>
#include <vector>
#include <sstream>
#include "argh.h"

#include <vfs/api/vfs_cxx_api.hh>

static std::vector<std::string> split_winmain(const std::string& input)
{
  std::vector<std::string> result;

  std::string::const_iterator i = input.begin(), e = input.end();
  for(;i != e; ++i)
    if (!std::isspace((unsigned char)*i))
      break;

  if (i != e) {

    std::string current;
    bool inside_quoted = false;
    bool empty_quote = false;
    int backslash_count = 0;

    for(; i != e; ++i) {
      if (*i == '"') {
        // '"' preceded by even number (n) of backslashes generates
        // n/2 backslashes and is a quoted block delimiter
        if (backslash_count % 2 == 0) {
          current.append(backslash_count / 2, '\\');
          empty_quote = inside_quoted && current.empty();
          inside_quoted = !inside_quoted;
          // '"' preceded by odd number (n) of backslashes generates
          // (n-1)/2 backslashes and is literal quote.
        } else {
          current.append(backslash_count / 2, '\\');
          current += '"';
        }
        backslash_count = 0;
      } else if (*i == '\\') {
        ++backslash_count;
      } else {
        // Not quote or backslash. All accumulated backslashes should be
        // added
        if (backslash_count) {
          current.append(backslash_count, '\\');
          backslash_count = 0;
        }
        if (std::isspace((unsigned char)*i) && !inside_quoted) {
          // Space outside quoted section terminate the current argument
          result.push_back(current);
          current.resize(0);
          empty_quote = false;
          for(;i != e && std::isspace((unsigned char)*i); ++i)
            ;
          --i;
        } else {
          current += *i;
        }
      }
    }

    // If we have trailing backslashes, add them
    if (backslash_count)
      current.append(backslash_count, '\\');

    // If we have non-empty 'current' or we're still in quoted
    // section (even if 'current' is empty), add the last token.
    if (!current.empty() || inside_quoted || empty_quote)
      result.push_back(current);
  }
  return result;
}


namespace vfs::api::cmd_opts {

  struct arg_info {
    enum type_t {
      eBOOLEAN,
      eSTRING,
      ePOSITIONAL
    };
    std::string m_short_name;
    std::string m_long_name;
    type_t      m_type;
    int         m_position {0};
  };

  struct cmd_line::impl {
    std::vector<arg_info> m_args;
    int m_pos {0};
    argh::parser m_parser;
    mutable std::string m_description;
  };

  cmd_line::cmd_line() {
    m_pimpl = std::make_unique<impl>();
  }

  cmd_line::~cmd_line() = default;

  cmd_line& cmd_line::add_switch(const std::string& short_name, const std::string& long_name) {
    m_pimpl->m_args.push_back ({short_name, long_name, arg_info::eBOOLEAN});
    return *this;
  }

  cmd_line& cmd_line::add_parameter(const std::string& short_name, const std::string& long_name) {
    m_pimpl->m_args.push_back ({short_name, long_name, arg_info::eSTRING});
    return *this;
  }

  cmd_line& cmd_line::add_positional(const std::string& short_name) {
    m_pimpl->m_args.push_back ({short_name, "", arg_info::ePOSITIONAL, m_pimpl->m_pos++});
    return *this;
  }

  void cmd_line::parse(const std::string& args) {
      auto args_v = split_winmain (args);
      std::unique_ptr<const char*[]> c_args(new const char*[args_v.size()]);
      for (std::size_t i=0; i<args_v.size(); i++) {
        c_args[i] = args_v[i].c_str();
      }
      m_pimpl->m_parser.parse (static_cast<int>(args_v.size()), c_args.get());
  }

  const std::string& cmd_line::describe() const {
    if (m_pimpl->m_description.empty()) {
      std::ostringstream os;
      bool first = true;
      for (const auto& arg: m_pimpl->m_args) {
        if (arg.m_type == arg_info::ePOSITIONAL) {
          continue;
        }
        if (first) {
          first = false;
        } else {
          os << " ";
        }
        os << "<" << arg.m_short_name << "|" << arg.m_long_name;
        if (arg.m_type == arg_info::eSTRING) {
          os << "=value>";
        } else {
          os <<">";
        }
      }
      for (const auto& arg: m_pimpl->m_args) {
        if (arg.m_type != arg_info::ePOSITIONAL) {
          continue;
        }
        if (first) {
          first = false;
        } else {
          os << " ";
        }
        os << "<" << arg.m_short_name << ">";
      }
      m_pimpl->m_description = os.str();
    }
    return m_pimpl->m_description;
  }

  std::optional<std::string> cmd_line::get(const std::string& short_name) const {
    for (const auto& arg : m_pimpl->m_args) {
      if (short_name == arg.m_short_name) {
        if (arg.m_type != arg_info::ePOSITIONAL) {
          if (arg.m_type == arg_info::eBOOLEAN) {
            auto value = m_pimpl->m_parser[{arg.m_short_name.c_str (), arg.m_long_name.c_str ()}];
            if (value) {
              return "true";
            } else {
              return "";
            }
          } else {
            std::string value;
            bool absent = !(m_pimpl->m_parser ({arg.m_short_name.c_str (), arg.m_long_name.c_str ()}) >> value);
            if (absent) {
              return std::nullopt;
            }
            return value;
          }
        } else {
          std::string value;
          bool absent = !(m_pimpl->m_parser (arg.m_position) >> value);
          if (absent) {
            return std::nullopt;
          }
          return value;
        }
      }
    }
    return std::nullopt;
  }
}
