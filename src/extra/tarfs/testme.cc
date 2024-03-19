//
// Created by igor on 3/18/24.
//

#include <iostream>
#include <fstream>
#include "microtar.h"
#include <tar_stream.hh>
#include "tar_archive.hh"


int main(int argc, char* argv[]) {
	std::ifstream ifs("/home/igor/proj/ares/vfs/test_data/tarfs/disk.tar", std::ios::binary);

	auto tar = vfs::extra::create_mtar_from_stream (&ifs);
	if (MTAR_ESUCCESS != mtar_open_stream (tar.get(), "r")) {
		std::cerr << "ZOPA" << std::endl;
		return 1;
	}
	mtar_header_t h;
	while ( (mtar_read_header(tar.get(), &h)) != MTAR_ENULLRECORD ) {

		int is_dir = h.type == MTAR_TDIR;
		printf("%s (%d bytes) is_dir = %d pos = %lu\n", h.name, h.size, is_dir, h.data_pos);
		mtar_next(tar.get());
	}
//	vfs::extra::tarfile tf(ifs);
	return 0;
}
