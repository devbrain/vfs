//
// Created by igor on 3/19/24.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>
#include "input_stream.hh"
#include "lhasa/public/lhasa.h"

std::filesystem::path result_path(const LHAFileHeader* header) {
	std::filesystem::path dir_path;
	if (header->path) {
		dir_path = std::filesystem::path(header->path);
	}
	std::filesystem::path file_path;
	if (header->filename) {
		file_path = std::filesystem::path(header->filename);
	}
	if (dir_path.empty()) {
		return file_path;
	}
	if (file_path.is_absolute()) {
		return file_path;
	}
	return dir_path / file_path;
}

struct entry_info {
	entry_info (uint64_t pos, size_t length, std::ifstream* stream)
		: pos (pos), length (length), stream (stream) {}

	uint64_t pos;
	std::size_t length;
	std::ifstream* stream;
};

std::size_t decoder_callback(void *buf, size_t buf_len,
							 void *user_data) {
	auto* e = reinterpret_cast<entry_info*>(user_data);
	std::size_t to_read = std::min(buf_len, e->length);
	if (to_read > 0) {
		auto current = e->stream->tellg();
		e->stream->seekg (e->pos, std::ios::beg);
		e->stream->read ((char*)buf, to_read);
		auto now = e->stream->tellg();
		auto has_bytes = now - current;
		e->pos += has_bytes;
		e->length -= has_bytes;
		e->stream->seekg (current, std::ios::beg);
		return has_bytes;
	}
	return 0;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	std::ifstream ifs("/home/igor/proj/ares/vfs/test_data/lhafs/test.lha", std::ios::binary | std::ios::in);
	//std::ifstream ifs("/home/igor/tmp/lhasa-master/test/archives/larc333/subdir.lzs", std::ios::binary | std::ios::in);
	if (!ifs) {
		std::cerr << "Cant open file" << std::endl;
		return 1;
	}
	auto rdr_ops = vfs::extra::create_lha_istream();
	auto stream = lha_input_stream_new (&rdr_ops, &ifs);
	auto reader = lha_reader_new (stream);

	LHAFileHeader* header = nullptr;
	do {
		header = lha_reader_next_file (reader);
		if (!header) {
			break;
		}
		auto pos = ifs.tellg();
		int type = 0;
		if (strcmp(header->compress_method, LHA_COMPRESS_TYPE_DIR) == 0) {
			type = 1;
		} else {
			if (header->symlink_target) {
				type = 2;
			}
		}
		std::cout << type << ")" << result_path(header) << pos << ":" << header->compressed_length << std::endl;
		entry_info e(pos, header->compressed_length, &ifs);
		auto* decoder_type = lha_decoder_for_name (header->compress_method);
		auto* decoder = lha_decoder_new (decoder_type, decoder_callback, &e, header->length);
		unsigned char buf[64];
		size_t sz = lha_decoder_read (decoder, buf, sizeof (buf));
		lha_decoder_free (decoder);
	} while (header);

	lha_reader_free (reader);
	lha_input_stream_free (stream);
	return 0;
}