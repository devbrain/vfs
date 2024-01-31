#include "sandbox.hh"
#include <vfs/system.hh>
#include <fstream>
#include <iostream>

sandbox::sandbox () {
	auto tmp = std::filesystem::temp_directory_path ();
	_root = tmp / std::filesystem::path ("unittest");

	if (std::filesystem::exists (_root)) {
		std::filesystem::remove_all (_root);
	}
	std::filesystem::create_directories (_root);
}

sandbox::~sandbox () {
	if (std::filesystem::exists (_root)) {
		std::filesystem::remove_all (_root);
	}
	vfs::deinitialize ();
}

void sandbox::mkdir (const std::string& pth) {
	auto path = _root / std::filesystem::path (pth);
	std::filesystem::create_directories (path);
}

void sandbox::create_file (const std::string& pth, const std::string& text) {
	auto path = _root / std::filesystem::path (pth);
	std::ofstream file (path); //open in constructor
	file << text;
}

std::string sandbox::root () const noexcept {
	return _root.generic_string ();
}