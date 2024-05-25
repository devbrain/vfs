//
// Created by igor on 3/31/24.
//

#ifndef VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_FORMAT_INFO_HH_
#define VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_FORMAT_INFO_HH_

#include "7zip-wrapper/fake_windows.hh"

#include <optional>
#include <string>
#include <cstdint>
#include <vector>

namespace z7 {
	struct format_info {
		std::string name;
		GUID class_id;
		bool update_enabled;
		bool keep_name;

		std::vector<std::string> exts;
		std::vector<std::string> add_exts;

		std::vector<char> start_signature;
		std::vector<char> finish_signature;


		UInt32 signature_offset;
    	std::vector<std::vector<char>> signatures;

		uint32_t format_index;
	};

	std::vector<format_info> get_formats();
}

#endif //VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_FORMAT_INFO_HH_
