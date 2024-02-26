//
// Created by igor on 2/18/24.
//
#include <doctest/doctest.h>
#include <vfs/extra/zipfs.hh>
#include <vfs/archive.hh>
#include "utils/test_data.hh"

TEST_SUITE("zipfs test") {
	TEST_CASE ("Test stat") {
		vfs::archive zip(vfs::extra::create_zipfs(), get_test_file ("zipfs/test.zip"));

		auto s1 = zip.get_stats ("/test");
		REQUIRE(s1.has_value());
		REQUIRE(s1->size == 0);
		REQUIRE(s1->type == vfs::stats::eDIRECTORY);

		auto s2 = zip.get_stats ("/test/a");
		REQUIRE(s2.has_value());
		REQUIRE(s2->size == 0);
		REQUIRE(s2->type == vfs::stats::eDIRECTORY);

		auto s3 = zip.get_stats ("/test/a/b");
		REQUIRE(s3.has_value());
		REQUIRE(s3->size == 0);
		REQUIRE(s3->type == vfs::stats::eDIRECTORY);

		auto s4 = zip.get_stats ("/test/a/b/c");
		REQUIRE(s4.has_value());
		REQUIRE(s4->size == 0);
		REQUIRE(s4->type == vfs::stats::eDIRECTORY);

		auto s5 = zip.get_stats ("/test/a/b/c/2.dat");
		REQUIRE(s5.has_value());
		REQUIRE(s5->size == 2097152);
		REQUIRE(s5->type == vfs::stats::eFILE);


		auto s6 = zip.get_stats ("/test/a/b/1.dat");
		REQUIRE(s6.has_value());
		REQUIRE(s6->size == 1048576);
		REQUIRE(s6->type == vfs::stats::eFILE);


		auto s7 = zip.get_stats ("/test/a/256.dat");
		REQUIRE(!s7.has_value());

		auto s8 = zip.get_stats ("/test/256.dat");
		REQUIRE(s8.has_value());
		REQUIRE(s8->size == 262144);
		REQUIRE(s8->type == vfs::stats::eFILE);
	}
}
