//
// Created by igor on 25/12/2019.
//

#include "vfs/api/vfs_module.h"

namespace vfs
{
	namespace module
	{
		class physfs : public filesystem
		{
		public:
			explicit physfs(vfs_module* output)
				: filesystem(output, "physfs")
			{

			}

		private:
			inode* load_root (const std::string& params) override
			{
				return nullptr;
			}

			size_t max_name_length() override
			{
				return 64;
			}
		};
	} // ns module
} // ns vfs

REGISTER_FVS_MODULE(vfs::module::physfs)





