//
// Created by igor on 2/18/24.
//
#include <doctest/doctest.h>
#include <vfs/extra/zipfs.hh>
#include <vfs/archive.hh>
#include "test_data.hh"

TEST_SUITE("zipfs test") {
	TEST_CASE ("Test 1") {
		vfs::archive zip(vfs::extra::create_zipfs(), get_test_file ("zipfs/test.zip"));
	}
}
