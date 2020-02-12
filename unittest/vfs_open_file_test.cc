#include <gtest/gtest.h>
#include <vfs/api/system.hh>
#include <vfs/api/exception.hh>
#include "sandbox.hh"

TEST(OpenFileTest, openExisting) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");

    vfs::file* f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eOPEN_EXISTING, true);
    vfs::close(f);

    vfs::deinitialize();
}

TEST(OpenFileTest, openTruncate) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");


    // throw here because readonly
    EXPECT_THROW(vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eTRUNCATE_EXISTING, true), vfs::exception);

    vfs::file* f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eTRUNCATE_EXISTING, false);
    vfs::close(f);


    auto st = vfs::get_stats("/zopa/pizda/1.txt");
    EXPECT_TRUE(st);
    EXPECT_EQ(0, st->size);
    vfs::deinitialize();
}
