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
    EXPECT_TRUE(st.has_value());
    EXPECT_TRUE(st->type == vfs::stats::eDIRECTORY);

	st = vfs::get_stats("/zopa/pizda");
	EXPECT_TRUE(st.has_value());
	EXPECT_TRUE(st->type == vfs::stats::eDIRECTORY);

	st = vfs::get_stats("/zopa/pizda/1.txt");
	EXPECT_TRUE(st.has_value());
	EXPECT_TRUE(st->type == vfs::stats::eFILE);

	st = vfs::get_stats("/zopa/pizda/2.txt");
	EXPECT_FALSE(st.has_value());

}


