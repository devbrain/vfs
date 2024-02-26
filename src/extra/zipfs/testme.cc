//
// Created by igor on 2/5/24.
//
#include <iostream>
#include "miniz.h"

int main(int argc, char* argv[]) {
	mz_zip_archive zip_archive = {0};

	auto status = mz_zip_reader_init_file(&zip_archive, "/home/igor/tmp/test.zip", 0);
	if (!status) {
		auto err = mz_zip_get_last_error (&zip_archive);
		std::cout << "Error " << err << std::endl;
		return 1;
	}
	auto num_files = mz_zip_reader_get_num_files(&zip_archive);
	for (int i = 0; i < num_files; i++)
	{
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
		{
			printf("mz_zip_reader_file_stat() failed!\n");
			mz_zip_reader_end(&zip_archive);
			return EXIT_FAILURE;
		}

		printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u, Is Dir: %u\n", file_stat.m_filename, file_stat.m_comment, (uint)file_stat.m_uncomp_size, (uint)file_stat.m_comp_size, mz_zip_reader_is_file_a_directory(&zip_archive, i));

		if (!strcmp(file_stat.m_filename, "test/256.dat"))
		{
			size_t pSize;
			mz_zip_reader_extract_file_to_heap (&zip_archive, "test/256.dat", &pSize, 0);
		}
	}

	// Close the archive, freeing any resources it was using
	mz_zip_reader_end(&zip_archive);
	return 0;
}