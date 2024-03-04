//
// Created by igor on 3/4/24.
//

#include <bsw/io/binary_reader.hh>
#include <fstream>

#include "bpb.hh"

int main(int argc, char* argv[]) {

	std::ifstream ifs("/home/igor/proj/ares/vfs/test_data/floppyfs/disk01.img", std::ios::binary);
	bsw::io::binary_reader istream(ifs, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);
	vfs::extra::bpb bpb{};
	istream >> bpb;
	auto root_start = bpb.reserved_sectors + bpb.number_of_fats*bpb.sectors_per_fat;
	auto cluster_start = root_start + (bpb.root_dir_entries*32) / bpb.bytes_per_sector;
	if ((bpb.root_dir_entries*32) % bpb.bytes_per_sector) {
		cluster_start++;
	}
	auto number_of_clusters = 2 + (bpb.total_sectors - cluster_start) / bpb.sectors_per_cluster;
	return 0;
}