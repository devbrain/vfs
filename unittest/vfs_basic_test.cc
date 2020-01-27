#include <gtest/gtest.h>
#include <vfs/api/system.hh>
#include <vfs/api/exception.hh>
#include "sandbox.hh"


TEST(BasicTest, testMount) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    vfs::load_module(stdfs::path("."));
	/*
    for (const auto& x : vfs::get_modules())
    {
        std::cout << x << std::endl;
    }
	*/

    EXPECT_THROW(vfs::mount("zopa", sbox.root(), "/"), vfs::exception);

    vfs::mount("physfs", sbox.root(), "/");

    /*
    for (const auto& x : vfs::get_mounts())
    {
        std::cout << x << std::endl;
    }
	*/

    auto st = vfs::get_stats("/zopa");
    EXPECT_TRUE(st);
    EXPECT_TRUE(st->type == vfs::stats::eDIRECTORY);

	st = vfs::get_stats("/zopa/pizda");
	EXPECT_TRUE(st);
	EXPECT_TRUE(st->type == vfs::stats::eDIRECTORY);

	st = vfs::get_stats("/zopa/pizda/1.txt");
	EXPECT_TRUE(st);
	EXPECT_TRUE(st->type == vfs::stats::eFILE);

	st = vfs::get_stats("/zopa/pizda/2.txt");
	EXPECT_FALSE(st);

	vfs::deinitialize();
}



TEST(BasicTest, testMount2) {
	sandbox sbox;
	sbox.mkdir("zopa/pizda");
	sbox.mkdir("a");
	sbox.create_file("zopa/pizda/1.txt", "aaa");


	vfs::load_module(stdfs::path("."));

	EXPECT_THROW(vfs::mount("zopa", sbox.root(), "/"), vfs::exception);

	vfs::mount("physfs", sbox.root(), "/");
	vfs::mount("physfs", sbox.root() + "/zopa", "/a");



	auto st = vfs::get_stats("/a/zopa");
	EXPECT_TRUE(!st);
	

	st = vfs::get_stats("/a/pizda");
	EXPECT_TRUE(st);
	EXPECT_TRUE(st->type == vfs::stats::eDIRECTORY);


	st = vfs::get_stats("/a/pizda/1.txt");
	EXPECT_TRUE(st);
	EXPECT_TRUE(st->type == vfs::stats::eFILE);

	st = vfs::get_stats("/a/pizda/2.txt");
	EXPECT_FALSE(st);

	for (const auto d : vfs::open_directory("/a"))
	{
		std::cout << std::get<0>(d) << std::endl;
	}

	vfs::deinitialize();
}



