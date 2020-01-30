#include <gtest/gtest.h>
#include <vfs/api/system.hh>
#include <vfs/api/exception.hh>
#include "sandbox.hh"

TEST(UnlinkTest, testUnlinkFile) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");
	sbox.create_file("zopa/pizda/2.txt", "aaa");


	vfs::load_module(stdfs::path("."));

	vfs::mount("physfs", sbox.root(), "/");
	vfs::mount("physfs", sbox.root() + "/zopa", "/a");

	vfs::unlink("/zopa/pizda/1.txt");
	auto st = vfs::get_stats("/zopa/pizda/1.txt");
	EXPECT_FALSE(st);

	st = vfs::get_stats("/a/pizda/1.txt");
	EXPECT_FALSE(st);

	vfs::deinitialize();
}

TEST(UnlinkTest, testUnlinkFileStat) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");
	sbox.create_file("zopa/pizda/2.txt", "aaa");


	vfs::load_module(stdfs::path("."));

	vfs::mount("physfs", sbox.root(), "/");
	vfs::mount("physfs", sbox.root() + "/zopa", "/a");

	// load /a/pizda/1.txt to dentry
	auto st = vfs::get_stats("/zopa/pizda/1.txt");
	EXPECT_TRUE(st);

	vfs::unlink("/zopa/pizda/1.txt");
	st = vfs::get_stats("/zopa/pizda/1.txt");
	EXPECT_FALSE(st);

	st = vfs::get_stats("/a/pizda/1.txt");
	EXPECT_FALSE(st);

	vfs::deinitialize();
}

TEST(UnlinkTest, testUnlinkFileMount) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");
	sbox.create_file("zopa/pizda/2.txt", "aaa");


	vfs::load_module(stdfs::path("."));

	vfs::mount("physfs", sbox.root(), "/");
	vfs::mount("physfs", sbox.root() + "/zopa", "/a");

	// load /a/pizda/1.txt to dentry
	auto st = vfs::get_stats("/a/pizda/1.txt");
	EXPECT_TRUE(st);

	vfs::unlink("/zopa/pizda/1.txt");
	st = vfs::get_stats("/zopa/pizda/1.txt");
	EXPECT_FALSE(st);

	st = vfs::get_stats("/a/pizda/1.txt");
	EXPECT_FALSE(st);

	vfs::deinitialize();
}
