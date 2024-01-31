#include <doctest/doctest.h>
#include <vfs/api/system.hh>
#include <vfs/api/exception.hh>
#include <bsw/macros.hh>
#include "sandbox.hh"

#define EXPECT_EQ(A,B) REQUIRE(A == B)
#define EXPECT_FALSE(A) REQUIRE(!(A))
#define EXPECT_TRUE(A) REQUIRE(A)
#define TEST(A,B) TEST_CASE(STRINGIZE(B))
#define EXPECT_THROW(A,B) REQUIRE_THROWS_AS(A, B)
#define EXPECT_ANY_THROW(A) REQUIRE_THROWS(A)

TEST(UnlinkTest, testUnlinkFile) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");
	sbox.create_file("zopa/pizda/2.txt", "aaa");


	vfs::load_module(std::filesystem::path("."));

	vfs::mount("physfs", sbox.root(), "/");
	vfs::mount("physfs", sbox.root() + "/zopa", "/a");

	vfs::unlink("/zopa/pizda/1.txt");
	auto st = vfs::get_stats("/zopa/pizda/1.txt");
	EXPECT_FALSE(st);

	st = vfs::get_stats("/a/pizda/1.txt");
	EXPECT_FALSE(st);

	vfs::deinitialize();
}

TEST(UnlinkTest, testUnlinkDirectory) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");

	vfs::load_module(std::filesystem::path("."));

	vfs::mount("physfs", sbox.root(), "/");

	vfs::unlink("/a");
	auto st = vfs::get_stats("/a");
	EXPECT_FALSE(st);

	st = vfs::get_stats("/zopa/pizda");
	EXPECT_TRUE(st);
	EXPECT_EQ(st->type, vfs::stats::eDIRECTORY);
	vfs::unlink("/zopa/pizda");
	EXPECT_FALSE(vfs::get_stats("/zopa/pizda"));


	vfs::deinitialize();
}

TEST(UnlinkTest, testUnlinkFileStat) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");
	sbox.create_file("zopa/pizda/2.txt", "aaa");


	vfs::load_module(std::filesystem::path("."));

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

TEST(UnlinkTest, testUnlinkFileInMem) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");
	sbox.create_file("zopa/pizda/2.txt", "aaa");


	vfs::load_module(std::filesystem::path("."));

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

TEST(UnlinkTest, testUnlinkNonEmpty)
{
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");


	vfs::load_module(std::filesystem::path("."));

	vfs::mount("physfs", sbox.root(), "/");

	EXPECT_ANY_THROW(vfs::unlink("/zopa"));
	EXPECT_TRUE(vfs::get_stats("/zopa"));

	EXPECT_ANY_THROW(vfs::unlink("/zopa/pizda"));
	vfs::unlink("/zopa/pizda/1.txt");
	vfs::unlink("/zopa/pizda/");
	EXPECT_FALSE(vfs::get_stats("/zopa/pizda"));
	vfs::deinitialize();

}

TEST(UnlinkTest, testUnlinkFileInMount) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");
	sbox.create_file("zopa/pizda/2.txt", "aaa");


	vfs::load_module(std::filesystem::path("."));


	vfs::mount("physfs", sbox.root() + "/zopa", "/");
	vfs::create_directory("/a");

	vfs::mount("physfs", sbox.root()+ "/a", "/a");

	bool a_found = false;
	for (const auto& m : vfs::get_mounts())
	{
		if (m.path() == "/a/")
		{
			a_found = true;
			break;
		}
	}
	EXPECT_TRUE(a_found);
	vfs::unlink("/a");
	a_found = false;
	for (const auto& m : vfs::get_mounts())
	{
		if (m.path() == "/a/")
		{
			a_found = true;
			break;
		}
	}
	EXPECT_FALSE(a_found);

	vfs::deinitialize();
}