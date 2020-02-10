/*
 Created by igor on 24/12/2019.
*/

#ifndef VFS_MODULE_H
#define VFS_MODULE_H

#if defined(__cplusplus)
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <algorithm>

#else
#include <stdint.h>
#include <stddef.h>
#endif

#if !defined(ssize_t) && !defined(__MINGW32__) && !defined(__MINGW64__)
#if defined(_WIN64) 
typedef __int64 ssize_t;
#else
typedef long ssize_t;
#endif
#endif

#if !defined(vfs_api_EXPORTS)
#define vfs_api_EXPORTS
#define vfs_api_EXPORTS_undef
#endif
#include "vfs/api/vfs_api.h"

#define VFS_MODULE_API VFS_API

#if defined(__cplusplus)
#define VFS_EXTERN_C extern "C"
#else
#define VFS_EXTERN_C
#endif

enum vfs_inode_type
{
	VFS_INODE_REGULAR,
	VFS_INODE_DIRECTORY,
	VFS_INODE_LINK
};

struct vfs_key_value
{
#define VFS_MAX_ATTRIB_LENGTH 64
	char key[VFS_MAX_ATTRIB_LENGTH];
	char value[VFS_MAX_ATTRIB_LENGTH];
};

struct vfs_inode_stats
{
	enum vfs_inode_type type;
	/*
	 * For file - size of the file
	 * For link and directories - 0
	 */
	uint64_t size;

	struct vfs_key_value attr1;
	struct vfs_key_value attr2;

	size_t num_of_additional_attributes;
	struct vfs_key_value* additional_attributes;

	/*
	 * de-allocates the additional_attributes
	 */
	void (* destructor)(struct vfs_inode_stats* victim);
};

/*
 * https://stackoverflow.com/questions/47403058/turning-the-next-hasnext-iterator-interface-into-begin-end-interface
 */
struct vfs_directory_iterator
{
	/*
	 * implementation specific data
	 */
	void* opaque;

	/*
	 * de-allocates the victim
	 */
	void (* destructor)(struct vfs_directory_iterator* victim);

	/*
	 * returns non-zero value if directory traversal not finished
	 */
	int (* has_next)(void* opaque);

	/*
	 * populates name with next directory entry name.
	 * returns length of name, or 0 if traversal finished
	 */
	size_t (* next)(void* opaque, char* name, size_t max_name_length);
};

enum whence_type
{
	eSEEK_START,
	eSEEK_SET,
	eSEEK_END
};

struct vfs_file_ops
{
	void* opaque;

	void (* destructor)(struct vfs_file_ops* victim);

	ssize_t (*read)  (void* opaque, void* buff, size_t len);
	ssize_t (*write) (void* opaque, void* buff, size_t len);
	int (*truncate) (void* opaque);
	/*
	 * Upon successful completion, lseek() returns the resulting offset location as measured in bytes
	 * from the beginning of the file.  On error, the value (uint64_t) -1
	 */
	uint64_t (*seek) (void* opaque, uint64_t pos, enum whence_type whence);
};

enum open_mode_type
{
	eREAD,
	eWRITE
};

struct vfs_inode_ops
{
	/*
	 * implementation specific data
	 */
	void* opaque;
	/*
	 * de-allocates the victim
	 */
	void (* destructor)(struct vfs_inode_ops* victim);



	/*
	 * performs lookup for named entry in this inode.
	 * return NULL if no such element found
	 */
	struct vfs_inode_ops* (* lookup)(void* opaque, char* name);

	/*
	 * populates output structure, returns 1 if OK, 0 on failure
	 */
	int (* stat)(void* opaque, struct vfs_inode_stats* output);


	/*
	 * returns directory iterator or NULL if this inode is not directory
	 */
	struct vfs_directory_iterator* (*get_directory_iterator) (void* opaque);

	int (* mkdir)(void* opaque, char* name);
	int (* unlink)(void* opaque);

	struct vfs_file_ops* (*open_file)(void* opaque, enum open_mode_type mode_type);
};

struct vfs_module
{
	void* opaque;

	void (* destructor)(struct vfs_module* victim);

	/*
	 * loads root inode for this file system.
	 */
	struct vfs_inode_ops* (* load_root)(void* opaque, char* params);

	/*
	 * returns maximal length for entry name
	 */
	size_t (* maximal_name_length)(void* opaque);

	size_t (* get_name)(void* opaque, char* output, size_t output_size);

	int (* sync_inode) (void* opaque, void* inode_opaque);
	int (* sync_filesystem) (struct vfs_module* fsys);
};

#define VFS_MODULE_REGISTER_NAME "vfs_module_register"

typedef void (* vfs_module_register_t)(struct vfs_module* output);

VFS_EXTERN_C void VFS_MODULE_API vfs_module_register(struct vfs_module* output);
/*
 * ==============================================================================
 */
#if defined(__cplusplus)
/*
 * C++ Part
 */
namespace vfs
{
	namespace module
	{
		class filesystem;

		class directory_iterator
		{
		public:
			directory_iterator() = default;
			virtual ~directory_iterator() = default;

			virtual bool has_next() = 0;
			virtual std::string next() = 0;
		public:
			static struct vfs_directory_iterator* vfs_directory_iterator_create(directory_iterator* obj);
		private:
			static void _destructor(struct vfs_directory_iterator* victim);
			static int _has_next(void* opaque);
			static size_t _next(void* opaque, char* name, size_t max_name_length);
		};

		class inode;

		class file
		{
		public:
			virtual ~file() = default;

			virtual ssize_t read  (void* buff, size_t len) = 0;
			virtual ssize_t write (void* buff, size_t len) = 0;
			virtual uint64_t seek (uint64_t pos, enum whence_type whence) = 0;
			virtual bool truncate() = 0;
		public:
			static struct vfs_file_ops* vfs_file_ops_create(file* obj);
		private:
			static void _destructor(struct vfs_file_ops* victim);

			static ssize_t _read (void* opaque, void* buff, size_t len);
			static ssize_t _write (void* opaque, void* buff, size_t len);
			static uint64_t _seek (void* opaque, uint64_t pos, enum whence_type whence);
            static int _truncate (void* opaque);
		};

		class inode
		{
			friend class filesystem;
		public:
			class stats
			{
			public:
				stats(vfs_inode_stats& output, vfs_inode_type type, uint64_t length);
				void attr(const char* key, const char* value);
			private:
				static void _destructor(vfs_inode_stats* out);
				static void _set_attr(vfs_key_value& out, const char* key, const char* value);
				static void _init_attr(vfs_key_value& out);
			private:
				vfs_inode_stats& _output;
				std::size_t _num_atts;
			};
		public:
			explicit inode(vfs_inode_type type);
			vfs_inode_type type() const;

			virtual ~inode() {
			}

			virtual void load_stat(stats& /*output*/)
			{
			};
			virtual inode* lookup(const char* name) = 0;
			virtual directory_iterator* get_directory_iterator() = 0;
			virtual uint64_t size() = 0;
			virtual bool mkdir(const char* name) = 0;
			virtual int unlink() = 0;

			virtual file* open_file(open_mode_type mode_type) = 0;
		public:
			static vfs_inode_ops* inode_create(vfs::module::inode* opaque);
		private:
			const vfs_inode_type _type;
		private:
			static void _destructor(struct vfs_inode_ops* victim);
			static struct vfs_inode_ops* _lookup(void* opaque, char* name);
			static int _stat(void* opaque, struct vfs_inode_stats* output);
			static struct vfs_directory_iterator* _get_directory_iterator(void* opaque);
			static int _mkdir(void* opaque, char* name);
			static int  _unlink(void* opaque);
			static vfs_file_ops* _open_file(void* opaque, enum open_mode_type mode_type);
		};
		
		class filesystem
		{
		public:
			explicit filesystem(const std::string& name);
			virtual ~filesystem() = default;

			virtual inode* load_root(const std::string& params) = 0;
			virtual size_t max_name_length() = 0;
			virtual int sync() = 0;
			virtual int sync_inode(inode* inod) = 0;

			virtual void setup(struct vfs_module* output);

		private:
			const std::string _name;

			static void _destructor(struct vfs_module* victim);
			static struct vfs_inode_ops* _load_root(void* opaque, char* params);
			static size_t _maximal_name_length(void* opaque);
			static size_t _get_name(void* opaque, char* output, size_t output_size);
			static int _sync_inode (void* opaque, void* inode_opaque);
			static int _sync_filesystem (struct vfs_module* victim);


		};
/* ============================================================================
	Implementation
   ============================================================================ */
		inline
		inode::stats::stats(vfs_inode_stats& output, vfs_inode_type type, uint64_t length)
			: _output(output),
			  _num_atts(0)
		{
			output.type = type;
			output.size = length;
			output.num_of_additional_attributes = 0;
			_init_attr(output.attr1);
			_init_attr(output.attr2);
			output.additional_attributes = nullptr;
			output.destructor = _destructor;
		}
		/* -------------------------------------------------------------------------- */
		inline
		void inode::stats::attr(const char* key, const char* value)
		{
			switch (_num_atts)
			{
			case 0:
				_set_attr(_output.attr1, key, value);
				_num_atts++;
				break;
			case 1:
				_set_attr(_output.attr2, key, value);
				_num_atts++;
				break;
			default:
				_output.num_of_additional_attributes++;
				if (!_output.additional_attributes)
				{
					_output.additional_attributes = static_cast<vfs_key_value*>(malloc(
						_output.num_of_additional_attributes * sizeof(vfs_key_value)));
				}
				else
				{
					void* new_ptr = realloc(_output.additional_attributes,
						_output.num_of_additional_attributes * sizeof(vfs_key_value));
					if (new_ptr)
					{
						_output.additional_attributes =
							static_cast<vfs_key_value*>(new_ptr);
					}
				}
				if (_output.additional_attributes)
				{
					_init_attr(_output.additional_attributes[_output.num_of_additional_attributes - 1]);
					_set_attr(_output.additional_attributes[_output.num_of_additional_attributes - 1], key, value);
				}
			}
		}
		/* -------------------------------------------------------------------------- */
		inline
		void inode::stats::_destructor(vfs_inode_stats* out)
		{
			if (out->additional_attributes != nullptr)
			{
				std::free(out->additional_attributes);
			}
		}
		/* -------------------------------------------------------------------------- */
		inline
		void inode::stats::_set_attr(vfs_key_value& out, const char* key, const char* value)
		{
			std::memcpy(out.key, key, std::min(std::strlen(key), sizeof(out.key)));
			std::memcpy(out.value, key, std::min(std::strlen(value), sizeof(out.value)));
		}
		/* -------------------------------------------------------------------------- */
		inline
		void inode::stats::_init_attr(vfs_key_value& out)
		{
			std::memset(out.value, 0, sizeof(out.value));
			std::memset(out.key, 0, sizeof(out.key));
		}
		/* ============================================================================= */
		inline
		inode::inode(vfs_inode_type type)
			: _type(type)
		{

		}
		/* ----------------------------------------------------------------------------- */
		inline
		vfs_inode_type inode::type() const
		{
			return _type;
		}
		/* ----------------------------------------------------------------------------- */
		inline
		vfs_inode_ops* inode::inode_create(vfs::module::inode* opaque)
		{
			if (!opaque)
			{
				return nullptr;
			}
			auto res = new vfs_inode_ops;

			res->opaque = opaque;
			res->destructor = _destructor;
			res->lookup = _lookup;
			res->get_directory_iterator = _get_directory_iterator;
			res->stat = _stat;
			res->mkdir = _mkdir;
			res->unlink = _unlink;
			return res;
		}
		/* ----------------------------------------------------------------------------- */
		inline
		void inode::_destructor(struct vfs_inode_ops* victim)
		{
			if (victim)
			{
				auto* ino = reinterpret_cast<vfs::module::inode*>(victim->opaque);
				delete ino;
			}
			delete victim;
		}
		/* ----------------------------------------------------------------------------- */
		inline
		struct vfs_inode_ops* inode::_lookup(void* opaque, char* name)
		{
			auto* ino = reinterpret_cast<vfs::module::inode*>(opaque);
			return inode_create(ino->lookup(name));
		}
		/* ----------------------------------------------------------------------------- */
		inline
		int inode::_stat(void* opaque, struct vfs_inode_stats* output)
		{
			auto* ino = reinterpret_cast<vfs::module::inode*>(opaque);
			try
			{
				const auto type = ino->type();
				uint64_t len = type == VFS_INODE_REGULAR ? ino->size() : 0;
				vfs::module::inode::stats st(*output, type, len);
				ino->load_stat(st);
				return 1;
			}
			catch (std::exception&)
			{
				return 0;
			}
		}
		/* ----------------------------------------------------------------------------- */
		inline
		int inode::_mkdir(void* opaque, char* name)
		{
			auto* ino = reinterpret_cast<vfs::module::inode*>(opaque);
			return ino->mkdir(name) ? 1 : 0;
		}
		/* ----------------------------------------------------------------------------- */
		inline
		int inode::_unlink(void* opaque)
		{
			auto* ino = reinterpret_cast<vfs::module::inode*>(opaque);
			return ino->unlink();
		}
		/* ----------------------------------------------------------------------------- */
		inline
		vfs_file_ops* inode::_open_file(void* opaque, enum open_mode_type mode_type)
		{
			auto* ino = reinterpret_cast<vfs::module::inode*>(opaque);
			return file::vfs_file_ops_create(ino->open_file(mode_type));
		}
		/* ----------------------------------------------------------------------------- */
		inline
		struct vfs_directory_iterator* inode::_get_directory_iterator(void* opaque)
		{
			auto* ino = reinterpret_cast<vfs::module::inode*>(opaque);
			if (ino->type() != VFS_INODE_DIRECTORY)
			{
				return nullptr;
			}
			return directory_iterator::vfs_directory_iterator_create(ino->get_directory_iterator());
		}


		/* ============================================================================= */
		inline
		filesystem::filesystem(const std::string& name)
			: _name(name)
		{

		}
		/* ----------------------------------------------------------------------------- */
		inline
		void filesystem::setup(struct vfs_module* output)
		{
			output->opaque = this;

			output->destructor = _destructor;
			output->load_root = _load_root;
			output->maximal_name_length = _maximal_name_length;
			output->get_name = _get_name;
			output->sync_filesystem = _sync_filesystem;
			output->sync_inode = _sync_inode;
		}
		/* ----------------------------------------------------------------------------- */
		inline
		void filesystem::_destructor(struct vfs_module* victim)
		{
			auto* fs = reinterpret_cast<vfs::module::filesystem*> (victim->opaque);
			delete fs;

			delete victim;
		}
		/* ----------------------------------------------------------------------------- */
		inline
		struct vfs_inode_ops* filesystem::_load_root(void* opaque, char* params)
		{
			auto* fs = reinterpret_cast<vfs::module::filesystem*> (opaque);
			return inode::inode_create(fs->load_root(params));
		}
		/* ----------------------------------------------------------------------------- */
		inline
		size_t filesystem::_maximal_name_length(void* opaque)
		{
			auto* fs = reinterpret_cast<vfs::module::filesystem*> (opaque);
			return fs->max_name_length();
		}
		/* ----------------------------------------------------------------------------- */
		inline
		size_t filesystem::_get_name(void* opaque, char* output, size_t output_size)
		{
			auto* fs = reinterpret_cast<vfs::module::filesystem*> (opaque);
			std::memcpy(output, fs->_name.c_str(), std::min(output_size, fs->_name.size()));
			return fs->_name.size();
		}
		/* ----------------------------------------------------------------------------- */
		inline
		int filesystem::_sync_inode (void* opaque, void* inode_opaque)
		{
			auto* fs = reinterpret_cast<vfs::module::filesystem*> (opaque);
			auto* ino = reinterpret_cast<vfs::module::inode*> (inode_opaque);
			return fs->sync_inode(ino);
		}
		/* ----------------------------------------------------------------------------- */
		inline
		int filesystem::_sync_filesystem (struct vfs_module* fsys)
		{
			auto* fs = reinterpret_cast<vfs::module::filesystem*> (fsys->opaque);
			return fs->sync();
		}
		/* ============================================================================= */
		inline
		struct vfs_directory_iterator* directory_iterator::vfs_directory_iterator_create(directory_iterator* obj)
		{
			vfs_directory_iterator* ret = new vfs_directory_iterator;
			ret->opaque = obj;
			ret->next = _next;
			ret->has_next = _has_next;
			ret->destructor = _destructor;
			return ret;
		}
		/* ----------------------------------------------------------------------------- */
		inline
		void directory_iterator::_destructor(struct vfs_directory_iterator* victim)
		{
			if (victim)
			{
				auto* obj = reinterpret_cast<vfs::module::directory_iterator*>(victim->opaque);
				delete obj;
			}
			delete victim;
		}
		/* ----------------------------------------------------------------------------- */
		inline
		int directory_iterator::_has_next(void* opaque)
		{
			auto* obj = reinterpret_cast<vfs::module::directory_iterator*>(opaque);
			return obj->has_next();
		}
		/* ----------------------------------------------------------------------------- */
		inline
		size_t directory_iterator::_next(void* opaque, char* name, size_t max_name_length)
		{
			auto* obj = reinterpret_cast<vfs::module::directory_iterator*>(opaque);
			auto nm = obj->next();
			if (!nm.empty())
			{
				std::memcpy(name, nm.c_str(), std::min(max_name_length, nm.size()));
			}
			return nm.size();
		}
		// =================================================================================
		inline
		vfs_file_ops* file::vfs_file_ops_create(file* obj)
		{
			if (!obj)
			{
				return nullptr;
			}
			vfs_file_ops* ret = new vfs_file_ops;
			ret->opaque = obj;
			ret->destructor = _destructor;
			ret->read = _read;
			ret->write = _write;
			ret->seek = _seek;
			ret->truncate = _truncate;

			return ret;
		}
		// ---------------------------------------------------------------------------------
		inline
		void file::_destructor(struct vfs_file_ops* victim)
		{
			if (victim)
			{
				auto* obj = reinterpret_cast<vfs::module::file*>(victim->opaque);
				delete obj;
			}
			delete victim;
		}
		// ---------------------------------------------------------------------------------
		inline
		ssize_t file::_read (void* opaque, void* buff, size_t len)
		{
			auto* obj = reinterpret_cast<vfs::module::file*>(opaque);
			return obj->read(buff, len);
		}
		// ---------------------------------------------------------------------------------
		inline
		ssize_t file::_write (void* opaque, void* buff, size_t len)
		{
			auto* obj = reinterpret_cast<vfs::module::file*>(opaque);
			return obj->write(buff, len);
		}
        // ---------------------------------------------------------------------------------
        inline
		int file::_truncate (void* opaque)
        {
            auto* obj = reinterpret_cast<vfs::module::file*>(opaque);
            return obj->truncate() ? 1 : 0;
        }
		// ---------------------------------------------------------------------------------
		inline
		uint64_t file::_seek (void* opaque, uint64_t pos, enum whence_type whence)
		{
			auto* obj = reinterpret_cast<vfs::module::file*>(opaque);
			return obj->seek(pos, whence);
		}
	} /* ns module */
}
/* forward declaration */
VFS_EXTERN_C void VFS_MODULE_API vfs_module_register(struct vfs_module* output);

#define REGISTER_VFS_MODULE(FILESYS_TYPE)                                            \
void vfs_module_register(struct vfs_module* output)                                  \
{                                                                                    \
    (new FILESYS_TYPE())->setup(output);                                             \
}


#endif

/*
 * ==============================================================================
 */

#if defined(vfs_api_EXPORTS_undef)
#undef vfs_api_EXPORTS
#endif

#endif //VFS_MODULE_H
