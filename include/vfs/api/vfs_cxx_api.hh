//
// Created by igor on 7/22/23.
//

#ifndef INCLUDE_VFS_API_VFS_CXX_API_HH
#define INCLUDE_VFS_API_VFS_CXX_API_HH

#include <string>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <chrono>
#include <optional>

#include <vfs/api/vfs_api.h>


namespace vfs::api {

  class file_object;
  class filesystem;
  class fs_module;

  class file_object_metadata {
    public:
      using time_stamp_t = std::chrono::time_point<std::chrono::system_clock>;
      enum class type_t {
          DIRECTORY,
          FILE,
          LINK
      };
    public:
      explicit file_object_metadata(filesystem* owner);
      virtual ~file_object_metadata() = default;

      virtual type_t get_type() const = 0;
      virtual uint64_t get_size() const = 0;
      virtual time_stamp_t get_creation_time() const = 0;
      virtual time_stamp_t get_modification_time() const = 0;

      virtual std::unique_ptr<file_object> open() = 0;

      [[nodiscard]] const filesystem* get_filesystem() const;
      filesystem* get_filesystem();

      void set_error(int error_code);
      void clear_error();


    private:
      filesystem* m_owner;
  };

  class file_object {
    public:
      explicit file_object(file_object_metadata* metadata);
      virtual ~file_object() = default;

      file_object_metadata* get_metadata();
      const file_object_metadata* get_metadata() const;

      virtual bool sync () = 0;

      void set_error(int error_code);
      void clear_error();
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
      virtual std::string get_target() = 0;
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
    private:
      fs_module* m_owner;
  };

  class fs_module {
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
    private:
      std::string m_name;
      vfs_error_module* m_error_module;
  };
  // ====================================================================================
  namespace detail {

    struct filesystem_bridge : vfs_api_filesystem {
      explicit filesystem_bridge(std::unique_ptr<filesystem> impl)
      : m_impl(std::move(impl)) {
      }
      private:
        std::unique_ptr<filesystem> m_impl;
    };

    struct fs_module_bridge : vfs_api_module {
      explicit fs_module_bridge(std::unique_ptr<fs_module> impl)
      : m_pimpl(std::move(impl)) {
        opaque = this;
        describe_parameters = _describe_parameters;
        get_name = _get_name;
        destroy = _destructor;
        create_filesystem = _create_fs;
        init_error_module = _init_error_module;
      }

      private:
        static const char* _describe_parameters(struct vfs_api_module* self) {
          return reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->describe_parameters();
        }

        static const char* _get_name(struct vfs_api_module* self) {
          return reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->get_name();
        }

        static void _destructor(struct vfs_api_module* self) {
          delete reinterpret_cast<fs_module_bridge*>(self);
        }

        static struct vfs_api_filesystem* _create_fs(struct vfs_api_module* self, const char* params) {
          auto fs_ptr = reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->create_filesystem (params);
          return nullptr;
        }

        static void _init_error_module(struct vfs_api_module* self, struct vfs_error_module* error_module) {
          reinterpret_cast<fs_module_bridge*>(self)->m_pimpl->set_error_module (error_module);
        }
      private:
        std::unique_ptr<fs_module> m_pimpl;
    };
  } // ns detail

  template <typename ModuleProvider>
  vfs_api_module* create_module() {
    return new detail::fs_module_bridge(std::make_unique<ModuleProvider>());
  }

#if !defined(BUILD_VFS_MODULE_AS_STATIC)
#define VFS_CPP_MODULE_FACTORY(ModuleProvider) VFS_MODULE_FACTORY { \
  return vfs::api::create_module<ModuleProvider>();                 \
}
#else
#define VFS_PPCAT_NX(A, B) A ## B
#define VFS_PPCAT(A, B) VFS_PPCAT_NX(A, B)

#define VFS_CPP_MODULE_FACTORY(ModuleProvider) vfs_api_module* VFS_PPCAT(create_, ModuleProvider) () { \
 return vfs::api::create_module<ModuleProvider>();                                     \
}
#endif
}

#endif //INCLUDE_VFS_API_VFS_CXX_API_HH
