//
// Created by igor on 3/4/24.
//

#include <bsw/io/binary_reader.hh>
#include <iostream>
#include <fstream>


#include "fat/driver.hh"
#include "fat/directory.hh"

int main(int argc, char* argv[]) {
	//std::ifstream ifs("/home/igor/tmp/fat12/images/floppy.img", std::ios::binary);
	std::ifstream ifs("/home/igor/proj/ares/vfs/test_data/floppyfs/Disk.img", std::ios::binary);
	//std::ifstream ifs("/home/igor/proj/ares/vfs/test_data/floppyfs/disk01.img", std::ios::binary);
	vfs::extra::driver driver(ifs);
	auto d = driver.get_directory (0);
	auto itr = d.get_iterator();
	std::optional<vfs::extra::directory::entry> e;
	do {
		e = itr.read();
		if (e.has_value()) {
			std::cout << *e << std::endl;
		}
	} while(e.has_value());

	return 0;
}