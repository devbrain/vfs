//
// Created by igor on 2/18/24.
//
#include <vector>
#include <cstring>
#include <doctest/doctest.h>
#include <vfs/extra/zipfs.hh>
#include <vfs/archive.hh>
#include <bsw/io/memory_stream_buf.hh>
#include "utils/test_data.hh"
#include "utils/sha1.hh"

TEST_SUITE("zipfs test") {
	TEST_CASE ("Test stat") {
		vfs::archive zip (vfs::extra::create_zipfs (), get_test_file ("zipfs/test.zip"));

		auto s1 = zip.get_stats ("/test");
		REQUIRE(s1.has_value ());
		REQUIRE(s1->size == 0);
		REQUIRE(s1->type == vfs::stats::eDIRECTORY);

		auto s2 = zip.get_stats ("/test/a");
		REQUIRE(s2.has_value ());
		REQUIRE(s2->size == 0);
		REQUIRE(s2->type == vfs::stats::eDIRECTORY);

		auto s3 = zip.get_stats ("/test/a/b");
		REQUIRE(s3.has_value ());
		REQUIRE(s3->size == 0);
		REQUIRE(s3->type == vfs::stats::eDIRECTORY);

		auto s4 = zip.get_stats ("/test/a/b/c");
		REQUIRE(s4.has_value ());
		REQUIRE(s4->size == 0);
		REQUIRE(s4->type == vfs::stats::eDIRECTORY);

		auto s5 = zip.get_stats ("/test/a/b/c/2.dat");
		REQUIRE(s5.has_value ());
		REQUIRE(s5->size == 2097152);
		REQUIRE(s5->type == vfs::stats::eFILE);

		auto s6 = zip.get_stats ("/test/a/b/1.dat");
		REQUIRE(s6.has_value ());
		REQUIRE(s6->size == 1048576);
		REQUIRE(s6->type == vfs::stats::eFILE);

		auto s7 = zip.get_stats ("/test/a/256.dat");
		REQUIRE(!s7.has_value ());

		auto s8 = zip.get_stats ("/test/256.dat");
		REQUIRE(s8.has_value ());
		REQUIRE(s8->size == 262144);
		REQUIRE(s8->type == vfs::stats::eFILE);
	}

	TEST_CASE("Test read simple") {
		vfs::archive zip (vfs::extra::create_zipfs (), get_test_file ("zipfs/test.zip"));
		auto st = zip.get_stats ("/test/256.dat");
		auto* f = zip.open ("/test/256.dat", vfs::READ_ONLY);
		REQUIRE(f != nullptr);
		if (!st->is_sequential) {

			vfs::seek (f, 0, vfs::seek_type::eEND);
			auto sz = vfs::tell (f);
			REQUIRE(sz == 262144);

			std::vector<unsigned char> x (sz, 0xFF);
			vfs::seek (f, 0, vfs::seek_type::eSET);
			vfs::read (f, x.data (), sz);
			for (const auto b : x) {
				if (b != 0) {
					REQUIRE(b == 0);
				}
			}
		} else {
			REQUIRE(st->size == 262144);
			char buff[256];
			size_t has_bytes = 0;
			while (has_bytes < st->size) {
				std::memset (buff, 0xFF, sizeof (buff));
				auto rc = vfs::read (f, buff, sizeof (buff));
				REQUIRE (rc >= 0);
				has_bytes += rc;
				for (size_t i = 0; i < (size_t)rc; i++) {
					auto b = buff[i];
					if (b != 0) {
						REQUIRE(b == 0);
					}
				}
			}
		}
		delete f;
	}

	TEST_CASE("Test read random data") {
		vfs::archive zip (vfs::extra::create_zipfs (), get_test_file ("zipfs/test.zip"));
		auto st = zip.get_stats ("/test/sample.txt");
		auto* f = zip.open ("/test/sample.txt", vfs::READ_ONLY);

		auto sz = st->size;
		REQUIRE(sz == 262144);

		std::vector<char> x (sz);
		size_t has_bytes = 0;
		while (has_bytes < st->size) {
			auto rc = vfs::read (f, x.data () + has_bytes, st->size - has_bytes);
			REQUIRE(rc >= 0);
			has_bytes += rc;
		}

		bsw::io::memory_input_stream stream (x.data (), x.size ());
		SHA1 sha_1;
		sha_1.update (stream);
		auto digest = sha_1.final ();
		REQUIRE(digest == "d0af266666e5bcbae63b6b3f63dc3f19bfbb8ac7");

		delete f;
	}
}
