//
// Created by igor on 3/19/24.
//

#include <iostream>
#include <fstream>
#include "input_stream.hh"
#include "lhasa/public/lhasa.h"



int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	// std::ifstream ifs("/home/igor/tmp/lhasa-master/test/archives/lharc_atari_313a/shorter.lzh", std::ios::binary | std::ios::in);
	std::ifstream ifs("/home/igor/tmp/lhasa-master/test/archives/larc333/subdir.lzs", std::ios::binary | std::ios::in);
	if (!ifs) {
		std::cerr << "Cant open file" << std::endl;
		return 1;
	}
	auto rdr_ops = vfs::extra::create_lha_istream();
	auto stream = lha_input_stream_new (&rdr_ops, &ifs);
	auto reader = lha_reader_new (stream);

	LHAFileHeader* header;
	do {
		header = lha_reader_next_file (reader);

		std::cout << "" << std::endl;
	} while (header);

	lha_reader_free (reader);
	lha_input_stream_free (stream);
	return 0;
}