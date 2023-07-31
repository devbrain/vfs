//
// Created by igor on 7/22/23.
//

#ifndef INCLUDE_VFS_API_VFS_CXX_API_HH
#define INCLUDE_VFS_API_VFS_CXX_API_HH

#include <string>
#include <sstream>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <utility>
#include <optional>
#include <functional>

#include <vfs/api/vfs_api.h>


namespace vfs::api {

  class file_object;
  class filesystem;
  class fs_module;

  class file_object_metadata {
    public:
      using time_stamp_t = time_t;
      enum class type_t {
          DIRECTORY,
          FILE,
          LINK
      };
    public:
      explicit file_object_metadata(filesystem* owner);
      virtual ~file_object_metadata() = default;

      [[nodiscard]] virtual type_t get_type() const = 0;
      [[nodiscard]] virtual uint64_t get_size() const = 0;
      [[nodiscard]] virtual time_stamp_t get_creation_time() const = 0;
      [[nodiscard]] virtual time_stamp_t get_modification_time() const = 0;
      [[nodiscard]] virtual const char* get_target() = 0;
      virtual void iterate(std::function<void(const std::string& name, const file_object_metadata& md)> itr) = 0;
      [[nodiscard]] virtual std::unique_ptr<file_object_metadata> load_entry(const std::string& name) = 0;
      virtual std::unique_ptr<file_object> open() = 0;

      [[nodiscard]] const filesystem* get_filesystem() const;
      filesystem* get_filesystem();

      void set_error(int error_code);
      void clear_error();

      template <typename ... Args>
      void log_debug(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_info(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_warn(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_error(const char* src_file, int line, Args...args);
    private:
      filesystem* m_owner;
  };

  class directory_metadata : public file_object_metadata {
    public:
      explicit directory_metadata(filesystem* owner);
      [[nodiscard]] type_t get_type() const override;
      [[nodiscard]] uint64_t get_size() const override;
      const char* get_target() override;
      std::unique_ptr<file_object> open() override;
  };

  class file_metadata : public file_object_metadata {
    public:
      explicit file_metadata(filesystem* owner);
      [[nodiscard]] type_t get_type() const override;
      const char* get_target() override;
      void iterate(std::function<void(const std::string& name, const file_object_metadata& md)> itr) override;
      [[nodiscard]] std::unique_ptr<file_object_metadata> load_entry(const std::string& name) override;
  };

  class link_metadata : public file_object_metadata {
    public:
      explicit link_metadata (filesystem* owner);

      [[nodiscard]] type_t get_type () const override;
      [[nodiscard]] uint64_t get_size () const override;
      void iterate (std::function<void (const std::string& name, const file_object_metadata& md)> itr) override;
      std::unique_ptr<file_object> open () override;
      [[nodiscard]] std::unique_ptr<file_object_metadata> load_entry(const std::string& name) override;
  };
  // =====================================================================================
  class file_object {
    public:
      explicit file_object(file_object_metadata* metadata);
      virtual ~file_object() = default;

      file_object_metadata* get_metadata();
      [[nodiscard]] const file_object_metadata* get_metadata() const;

      virtual bool sync () = 0;

      void set_error(int error_code);
      void clear_error();

      template <typename ... Args>
      void log_debug(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_info(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_warn(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_error(const char* src_file, int line, Args...args);
    private:
      file_object_metadata* m_metadata;
  };

  class file_object_directory : public file_object {
    public:
      explicit file_object_directory(file_object_metadata* metadata);

      virtual bool create_directory(const std::string& name) = 0;
      virtual bool remove (const std::string& name) = 0;
      virtual bool create_link(const std::string& source, const std::string& target) = 0;
  };

  class file_object_link: public file_object {
    public:
      explicit file_object_link(file_object_metadata* metadata);
      virtual const char* get_target() = 0;
  };

  class file_object_file: public file_object {
    public:
      explicit file_object_file(file_object_metadata* metadata);

      virtual std::optional<std::size_t> read(void* dst, std::size_t size) = 0;
      virtual std::optional<std::size_t> write(void* dst, std::size_t size) = 0;
      virtual std::optional<uint64_t> seek(uint64_t offset) = 0;
      virtual std::optional<uint64_t> tell() = 0;
  };

  class filesystem {
    public:
      explicit filesystem(fs_module* owner);
      virtual ~filesystem() = default;

      void set_error(int error_code);
      void clear_error();

      virtual std::unique_ptr<file_object_metadata> get_root() = 0;

      template <typename ... Args>
      void log_debug(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_info(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_warn(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_error(const char* src_file, int line, Args...args);

      fs_module* get_module();
    private:
      fs_module* m_owner;
  };
  namespace detail {
    struct fs_module_bridge;
    struct filesystem_bridge;
  }
  class fs_module {
      friend struct detail::fs_module_bridge;
      friend struct detail::filesystem_bridge;
    public:
      explicit fs_module(const char* name);
      virtual ~fs_module() = default;

      [[nodiscard]] const char* get_name() const;

      [[nodiscard]] virtual const char* describe_parameters() const = 0;
      virtual std::unique_ptr<filesystem> create_filesystem(const std::string& args) = 0;

      void set_error_module(vfs_error_module* error_module);
      virtual const char* error_to_string(int error_code) = 0;
      void set_error(int error_code);
      void clear_error();

      // logger

      void set_logger_module(vfs_logger_module* logger_module);

      template <typename ... Args>
      void log_debug(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_info(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_warn(const char* src_file, int line, Args...args);

      template <typename ... Args>
      void log_error(const char* src_file, int line, Args...args);
    private:
      std::string m_name;
      vfs_error_module* m_error_module;
      vfs_logger_module* m_logger;
      vfs_api_module* m_module;
  };

  class logger : public vfs_logger_module {
    public:
      logger()
      : vfs_logger_module {} {
        opaque = this;

        debug = fn_debug;
        is_debug_enabled = fn_is_debug_enabled;

        info = fn_info;
        is_info_enabled = fn_is_info_enabled;

        warn = fn_warn;
        is_warn_enabled = fn_is_warn_enabled;

        error = fn_error;
        is_error_enabled = fn_is_error_enabled;
      }
      virtual ~logger() = default;
    protected:
      virtual void _debug(const char* module_name, const char* source_file, int line, const char* string) = 0;
      [[nodiscard]] virtual bool _is_debug_enabled() const = 0;

      virtual void _info(const char* module_name, const char* source_file, int line, const char* string) = 0;
      [[nodiscard]] virtual bool _is_info_enabled() const = 0;

      virtual void _warn(const char* module_name, const char* source_file, int line, const char* string) = 0;
      [[nodiscard]] virtual bool _is_warn_enabled() const = 0;

      virtual void _error(const char* module_name, const char* source_file, int line, const char* string) = 0;
      [[nodiscard]] virtual bool _is_error_enabled() const = 0;
    private:
      static void fn_debug(struct vfs_logger_module* self, const char* module_name, const char* source_file, int line, const char* string) {
        ((logger*)(self))->_debug (module_name, source_file, line, string);
      }

      static int fn_is_debug_enabled(struct vfs_logger_module* self) {
        return ((logger*)(self))->_is_debug_enabled();
      }

      static void fn_info(struct vfs_logger_module* self, const char* module_name, const char* source_file, int line, const char* string) {
        ((logger*)(self))->_info (module_name, source_file, line, string);
      }

      static int fn_is_info_enabled(struct vfs_logger_module* self) {
        return ((logger*)(self))->_is_info_enabled();
      }

      static void fn_warn(struct vfs_logger_module* self, const char* module_name, const char* source_file, int line, const char* string) {
        ((logger*)(self))->_warn (module_name, source_file, line, string);
      }

      static int fn_is_warn_enabled(struct vfs_logger_module* self) {
        return ((logger*)(self))->_is_warn_enabled();
      }

      static void fn_error(struct vfs_logger_module* self, const char* module_name, const char* source_file, int line, const char* string) {
        ((logger*)(self))->_error (module_name, source_file, line, string);
      }

      static int fn_is_error_enabled(struct vfs_logger_module* self) {
        return ((logger*)(self))->_is_error_enabled();
      }
  };

#define VFS_LOG_DEBUG(...) this->log_debug(__FILE__, __LINE__, ##__VA_ARGS__)
#define VFS_LOG_INFO(...)  this->log_info(__FILE__, __LINE__, ##__VA_ARGS__)
#define VFS_LOG_WARN(...)  this->log_warn(__FILE__, __LINE__, ##__VA_ARGS__)
#define VFS_LOG_ERROR(...) this->log_error(__FILE__, __LINE__, ##__VA_ARGS__)



  // ===============================================================================
  namespace cmd_opts {
    class cmd_line {
      public:
        cmd_line();
        ~cmd_line();

        cmd_line& add_switch(const std::string& short_name, const std::string& long_name);
        cmd_line& add_parameter(const std::string& short_name, const std::string& long_name);
        cmd_line& add_positional(const std::string& short_name);

        void parse(const std::string& args);

        [[nodiscard]] std::optional<std::string> get(const std::string& short_name) const;

        [[nodiscard]] const std::string& describe() const;
      private:
        struct impl;
        std::unique_ptr<impl> m_pimpl;
    };
  }
  // ===============================================================================
  namespace detail {

    struct metadata_bridge : public vfs_api_dentry {
      public:
        explicit metadata_bridge(std::unique_ptr<file_object_metadata> impl);
        explicit metadata_bridge(file_object_metadata* impl);
        ~metadata_bridge();
      private:
        void _setup (const file_object_metadata* impl);
      private:
        static vfs_api_dentry_type _get_type(struct vfs_api_dentry* self);

        static time_t _get_ctime(struct vfs_api_dentry* self);
        static time_t _get_mtime(struct vfs_api_dentry* self);

        static uint64_t _get_size(struct vfs_api_dentry* self);

        static const char* _get_target(struct vfs_api_dentry* self);

        static void _iterate(struct vfs_api_dentry* self, void* context, vfs_api_directory_iterator_t iterator);

        static vfs_api_dentry* _load_dentry(struct vfs_api_dentry* self, const char* name);
        static void _destroy(vfs_api_dentry* self);
      private:
        std::unique_ptr<file_object_metadata> m_impl;
        bool m_is_owner;
    };

    struct filesystem_bridge : public vfs_api_filesystem {
      explicit filesystem_bridge(std::unique_ptr<filesystem> impl);
      private:
        static void _destroy(vfs_api_filesystem* self);
        static vfs_api_dentry* _get_root(struct vfs_api_filesystem* self);
        static vfs_api_module* _get_module(struct vfs_api_filesystem* self);
      private:
        std::unique_ptr<filesystem> m_impl;
    };

    struct fs_module_bridge : vfs_api_module {
      explicit fs_module_bridge(std::unique_ptr<fs_module> impl);
      private:
        static const char* _describe_parameters(struct vfs_api_module* self);
        static const char* _get_name(struct vfs_api_module* self);
        static void _destructor(struct vfs_api_module* self);
        static struct vfs_api_filesystem* _create_fs(struct vfs_api_module* self, const char* params);
        static void _init_error_module(struct vfs_api_module* self, struct vfs_error_module* error_module);
        static void _init_logger_module(struct vfs_api_module* self, struct vfs_logger_module* logger_module);
        static const char* _error_to_string (struct vfs_api_module* self, int error_code);
        static vfs_error_module* _get_error_module(struct vfs_api_module* self);
      private:
        std::unique_ptr<fs_module> m_pimpl;
    };
  } // ns detail

  // ====================================================================================
  // Implementation
  // ====================================================================================

#define d_DECLARE_LOGGER_IMPL(METHOD)                                                   \
  template <typename ... Args>                                                          \
  inline                                                                                \
  void fs_module::log_ ## METHOD (const char* src_file, int line, Args...args) {        \
    if (m_logger && m_logger->is_ ## METHOD ## _enabled(m_logger)) {                    \
      std::ostringstream os;                                                            \
      if constexpr (sizeof...(args) > 0) {                                              \
        ((os << ' ' << std::forward<Args> (args)), ...);                                \
      }                                                                                 \
      m_logger-> METHOD (m_logger, get_name(), src_file, line, os.str().c_str());       \
    }                                                                                   \
  }

  d_DECLARE_LOGGER_IMPL(debug)
  d_DECLARE_LOGGER_IMPL(info)
  d_DECLARE_LOGGER_IMPL(warn)
  d_DECLARE_LOGGER_IMPL(error)

#undef d_DECLARE_LOGGER_IMPL

#define d_DECLARE_LOGGER_CALL(CLASS, MEMBER, METHOD)                              \
  template <typename ... Args>                                                    \
  inline                                                                          \
  void CLASS::METHOD(const char* src_file, int line, Args... args) {              \
    MEMBER->METHOD (src_file, line, std::forward<Args>(args)...);                 \
  }

  d_DECLARE_LOGGER_CALL(filesystem, m_owner, log_debug)
  d_DECLARE_LOGGER_CALL(filesystem, m_owner, log_info)
  d_DECLARE_LOGGER_CALL(filesystem, m_owner, log_warn)
  d_DECLARE_LOGGER_CALL(filesystem, m_owner, log_error)

  d_DECLARE_LOGGER_CALL(file_object_metadata, m_owner, log_debug)
  d_DECLARE_LOGGER_CALL(file_object_metadata, m_owner, log_info)
  d_DECLARE_LOGGER_CALL(file_object_metadata, m_owner, log_warn)
  d_DECLARE_LOGGER_CALL(file_object_metadata, m_owner, log_error)

  d_DECLARE_LOGGER_CALL(file_object, m_metadata, log_debug)
  d_DECLARE_LOGGER_CALL(file_object, m_metadata, log_info)
  d_DECLARE_LOGGER_CALL(file_object, m_metadata, log_warn)
  d_DECLARE_LOGGER_CALL(file_object, m_metadata, log_error)

#undef d_DECLARE_LOGGER_CALL

  template <typename ModuleProvider>
  vfs_api_module* create_module() {
    return new detail::fs_module_bridge(std::make_unique<ModuleProvider>());
  }

#if !defined(BUILD_VFS_MODULE_AS_STATIC)
#define VFS_CPP_MODULE_FACTORY_DECLARE(ModuleProvider) VFS_MODULE_FACTORY
#define VFS_CPP_MODULE_FACTORY(ModuleProvider) VFS_CPP_MODULE_FACTORY_DECLARE(ModuleProvider) { \
  return vfs::api::create_module<ModuleProvider>();                 \
}
#else
#define VFS_PPCAT_NX(A, B) A ## B
#define VFS_PPCAT(A, B) VFS_PPCAT_NX(A, B)
#define VFS_CPP_MODULE_FACTORY_DECLARE(ModuleProvider) vfs_api_module* VFS_PPCAT(create_, ModuleProvider) ()
#define VFS_CPP_MODULE_FACTORY(ModuleProvider) VFS_CPP_MODULE_FACTORY_DECLARE(ModuleProvider) { \
 return vfs::api::create_module<ModuleProvider>();                                     \
}
#endif
}

#endif //INCLUDE_VFS_API_VFS_CXX_API_HH
