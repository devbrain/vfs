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
	void (*destructor) (struct vfs_inode_ops* victim);

	/*
	 * de-allocates the victim (stats)
	 */
	void (*stats_destructor) (void* opaque, struct vfs_inode_stats* victim);

	/*
	 * performs lookup for named entry in this inode.
	 * return NULL if no such element found
	 */
	struct vfs_inode_ops* (*lookup) (void* opaque, char* name);

	/*
	 * populates output structure, returns 1 if OK, 0 on failure
	 */
	int (*stat) (void* opaque, struct vfs_inode_stats* output);

	/*
	 * iterate over directory entries
	 *
	 * output - entry name should be written here. implementation should never de-allocate this name
	 * max_output_length - entry name should not exceed this length
	 * token - initial value is 0, provider should update this token as needed.
	 *         implementation should never de-allocate this token
	 *
	 * return value: size of output - if should continue, 0 - if end of the directory reached, < 0 on error
	 */
	int (*iterate_dir) (void* opaque, char* output, size_t max_output_length, uint64_t* token);
};

struct vfs_module
{
	void* opaque;

	void (*destructor) (struct vfs_module* victim);

	/*
	 * loads root inode for this file system.
	 */
	struct vfs_inode_ops* (*load_root)(void* opaque, char* params);

	/*
	 * returns maximal length for entry name
	 */
	size_t (*maximal_name_length)(void* opaque);

	size_t (*get_name)(void* opaque, char* output, size_t output_size);
};

#define VFS_MODULE_REGISTER_NAME "vfs_module_register"
typedef void (*vfs_module_register_t)(struct vfs_module* output);
VFS_EXTERN_C void VFS_MODULE_API vfs_module_register (struct vfs_module* output);
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
		class inode
		{
			friend class filesystem;
		public:
			class stats
			{
			public:
				stats(vfs_inode_stats& output, vfs_inode_type type, uint64_t length)
				: _output(output),
				_num_atts(0)
				{
					output.type = type;
					output.size = length;
					output.num_of_additional_attributes = 0;
					init_attr(output.attr1);
					init_attr(output.attr2);
					output.additional_attributes = nullptr;
				}

				void attr(const char* key, const char* value)
				{
					switch (_num_atts)
					{
					case 0:
						set_attr(_output.attr1, key, value);
						_num_atts++;
						break;
					case 1:
						set_attr(_output.attr2, key, value);
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
							init_attr(_output.additional_attributes[_output.num_of_additional_attributes - 1]);
							set_attr(_output.additional_attributes[_output.num_of_additional_attributes - 1], key, value);
						}
					}
				}
			public:
				static void destructor(vfs_inode_stats* out)
				{
					if (out->additional_attributes != nullptr)
					{
						std::free(out->additional_attributes);
					}
				}
			private:
				static void set_attr(vfs_key_value& out, const char* key, const char* value)
				{
					std::memcpy(out.key, key, std::min(std::strlen(key), sizeof(out.key)));
					std::memcpy(out.value, key, std::min(std::strlen(value), sizeof(out.value)));
				}
				static void init_attr(vfs_key_value& out)
				{
					std::memset(out.value, 0, sizeof(out.value));
					std::memset(out.key, 0, sizeof(out.key));
				}
			private:
				vfs_inode_stats& _output;
				std::size_t  _num_atts;
			};
		public:
			explicit inode (vfs_inode_type type)
			: _type (type)
			{

			}

			vfs_inode_type type() const
			{
				return _type;
			}

			virtual ~inode() = default;

			virtual void load_stat(stats& output) = 0;
			virtual inode* lookup(const char* name) = 0;
			virtual std::string iterate_directory(uint64_t& token) = 0;
			virtual uint64_t size () = 0;
		public:
			static vfs_inode_ops* inode_create(vfs::module::inode* opaque)
			{
				auto res = new vfs_inode_ops;

				res->opaque = opaque;
				res->destructor = _destructor;
				res->stats_destructor = _stats_destructor;

				res->lookup = _lookup;
				res->iterate_dir = _iterate_dir;
				res->stat = _stat;
				return res;
			}
		private:
			const vfs_inode_type _type;
		private:
			static void _destructor (struct vfs_inode_ops* victim)
			{
				auto* ino = reinterpret_cast<vfs::module::inode*>(victim->opaque);
				delete ino;
				delete victim;
			}

			static void _stats_destructor (void* /*opaque*/, struct vfs_inode_stats* victim)
			{
				vfs::module::inode::stats::destructor(victim);
			}

			static struct vfs_inode_ops* _lookup (void* opaque, char* name)
			{
				auto* ino = reinterpret_cast<vfs::module::inode*>(opaque);
				return inode_create(ino->lookup(name));
			}

			static int _stat (void* opaque, struct vfs_inode_stats* output)
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
				catch (std::exception& )
				{
					return 0;
				}
			}

			static int _iterate_dir (void* opaque, char* output, size_t max_output_length, uint64_t* token)
			{
				auto* ino = reinterpret_cast<vfs::module::inode*>(opaque);
				try
				{
					auto s = ino->iterate_directory(*token);
					if (s.empty())
					{
						return 0;
					}
					std::memcpy(output, s.c_str(), std::min(max_output_length, s.size()));
					return static_cast<int>(s.size());
				}
				catch (std::exception&)
				{
					return -1;
				}
			}
		};

		class filesystem
		{
		public:
			explicit filesystem(const std::string& name)
				: _name(name)
			{

			}
			virtual ~filesystem () = default;

			virtual inode* load_root (const std::string& params) = 0;
			virtual size_t max_name_length() = 0;

			virtual void setup(struct vfs_module* output)
			{
				output->opaque = this;

				output->destructor = _destructor;
				output->load_root = _load_root;
				output->maximal_name_length = _maximal_name_length;
				output->get_name = _get_name;
			}

		private:
			const std::string _name;



			static void _destructor (struct vfs_module* victim)
			{
				auto* fs = reinterpret_cast<vfs::module::filesystem*> (victim->opaque);
				delete fs;

				delete victim;
			}

			static struct vfs_inode_ops* _load_root(void* opaque, char* params)
			{
				auto* fs = reinterpret_cast<vfs::module::filesystem*> (opaque);
				return inode::inode_create(fs->load_root(params));
			}

			static size_t _maximal_name_length(void* opaque)
			{
				auto* fs = reinterpret_cast<vfs::module::filesystem*> (opaque);
				return fs->max_name_length();
			}

			static size_t _get_name(void* opaque, char* output, size_t output_size)
			{
				auto* fs = reinterpret_cast<vfs::module::filesystem*> (opaque);
				std::memcpy(output, fs->_name.c_str(), std::min(output_size, fs->_name.size()));
				return fs->_name.size();
			}
		};

	} /* ns module */
}

#define REGISTER_FVS_MODULE(FILESYS_TYPE)											\
VFS_EXTERN_C void VFS_MODULE_API vfs_module_register(struct vfs_module* output)		\
{																					\
	(new FILESYS_TYPE())->setup(output);							  			    	\
}


#endif

/*
 * ==============================================================================
 */

#if defined(vfs_api_EXPORTS_undef)
#undef vfs_api_EXPORTS
#endif

#endif //VFS_MODULE_H
