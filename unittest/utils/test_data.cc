//
// Created by igor on 2/18/24.
//

#include "test_data.hh"
#include <filesystem>

std::string get_test_file(const std::string& name) {
	std::filesystem::path base(VFS_TEST_DATA);
	return (base/name).u8string();
}