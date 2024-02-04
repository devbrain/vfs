#include <doctest/doctest.h>
#include <vfs/system.hh>
#include <vfs/io.hh>

#include "vfs/exception.hh"
#include "sandbox.hh"


TEST_CASE("test mount") {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    REQUIRE_THROWS_AS(vfs::mount("zopa", sbox.root(), "/"), vfs::exception);

    vfs::mount("physfs", sbox.root(), "/");

    auto st = vfs::get_stats("/zopa");
    REQUIRE(st);
    REQUIRE(st->type == vfs::stats::eDIRECTORY);

    st = vfs::get_stats("/zopa/pizda");
    REQUIRE(st);
    REQUIRE(st->type == vfs::stats::eDIRECTORY);

    st = vfs::get_stats("/zopa/pizda/1.txt");
    REQUIRE(st);
    REQUIRE(st->type == vfs::stats::eFILE);

    st = vfs::get_stats("/zopa/pizda/2.txt");
    REQUIRE(!st);

    vfs::deinitialize();
}



TEST_CASE("testMount2") {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.mkdir("a");
    sbox.create_file("zopa/pizda/1.txt", "aaa");

    REQUIRE_THROWS_AS(vfs::mount("zopa", sbox.root(), "/"), vfs::exception);

    vfs::mount("physfs", sbox.root(), "/");
    vfs::mount("physfs", sbox.root() + "/zopa", "/a");



    auto st = vfs::get_stats("/a/zopa");
    REQUIRE(!st);
	

    st = vfs::get_stats("/a/pizda");
    REQUIRE(st);
    REQUIRE(st->type == vfs::stats::eDIRECTORY);


    st = vfs::get_stats("/a/pizda/1.txt");
    REQUIRE(st);
    REQUIRE(st->type == vfs::stats::eFILE);

    st = vfs::get_stats("/a/pizda/2.txt");
    REQUIRE(!st);

    for (const auto d : vfs::open_directory("/a"))
	{
	    REQUIRE(std::get<0>(d) == "pizda");
	    REQUIRE(std::get<1>(d).type == vfs::stats::eDIRECTORY);
	}

    vfs::deinitialize();
}



TEST_CASE("testMkdir") {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.mkdir("a");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    REQUIRE_THROWS_AS(vfs::mount("zopa", sbox.root(), "/"), vfs::exception);

    vfs::mount("physfs", sbox.root(), "/");
    vfs::mount("physfs", sbox.root() + "/zopa", "/a");

    vfs::create_directory("/zopa/newDir");

    auto st = vfs::get_stats("/a/newDir");
    REQUIRE(st);
    REQUIRE(st->type == vfs::stats::eDIRECTORY);


    st = vfs::get_stats("/zopa/newDir");
    REQUIRE(st);
    REQUIRE(st->type == vfs::stats::eDIRECTORY);

    vfs::deinitialize();
}

