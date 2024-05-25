//
// Created by igor on 3/31/24.
//

#ifndef VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_CODECS_HH_
#define VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_CODECS_HH_

#include "7zip-wrapper/fake_windows.hh"

#include <optional>
#include <string>
#include <cstdint>
#include <vector>

namespace z7 {

	struct codec {
		codec (std::string  name,
			   const std::optional<GUID>& encoder,
			   const std::optional<GUID>& decoder,
			   uint32_t index);
		const std::string name;
		const std::optional<GUID> encoder;
		const std::optional<GUID> decoder;
		const uint32_t index;
	};

	std::vector<codec> get_codecs();
}


#endif //VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_CODECS_HH_
