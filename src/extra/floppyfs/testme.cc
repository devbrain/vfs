//
// Created by igor on 3/4/24.
//

#include <bsw/io/binary_reader.hh>
#include <iostream>
#include <fstream>

#include "fat/bpb.hh"
#include "fat/fat.hh"

int main(int argc, char* argv[]) {
	//std::ifstream ifs("/home/igor/tmp/fat12/images/floppy.img", std::ios::binary);
	std::ifstream ifs("/home/igor/proj/ares/vfs/test_data/floppyfs/Disk.img", std::ios::binary);
	auto bpb = vfs::extra::read_bpb (ifs);

	vfs::extra::fat fat(bpb.type, bpb.fat[0], ifs);
	for (uint32_t i = 0; i<fat.size(); i++) {
		auto c = fat.get (i);
		std::cout << std::hex << c << std::endl;
	}
	return 0;
}