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

TEST(OpenFileTest, openNonExisting) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");

    EXPECT_THROW(vfs::open("/zopa/pizda/for-read.txt", vfs::creation_disposition::eOPEN_EXISTING, true), vfs::exception);
    EXPECT_THROW(vfs::open("/zopa/pizda/for-write.txt", vfs::creation_disposition::eOPEN_EXISTING, false), vfs::exception);

    vfs::deinitialize();
}

TEST(OpenFileTest, openTruncate) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");

    vfs::file* f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eOPEN_EXISTING, true);
    vfs::seek(f, 0, vfs::seek_type::eEND);
    auto sz = vfs::tell(f);
    EXPECT_EQ(3, sz);
    vfs::close(f);

    // throw here because readonly
    EXPECT_THROW(vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eTRUNCATE_EXISTING, true), vfs::exception);

    f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eTRUNCATE_EXISTING, false);
    vfs::close(f);


    auto st = vfs::get_stats("/zopa/pizda/1.txt");
    EXPECT_TRUE(st);
    EXPECT_EQ(0, st->size);
    vfs::deinitialize();
}

TEST(OpenFileTest, CreateNew) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");

    EXPECT_THROW(vfs::open("/zopa/pizda/1aa.txt", vfs::creation_disposition::eCREATE_NEW, true), vfs::exception);

    vfs::file* f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eCREATE_NEW, true);
    vfs::seek(f, 0, vfs::seek_type::eEND);
    auto sz = vfs::tell(f);
    EXPECT_EQ(3, sz);
    vfs::close(f);

    f = vfs::open("/zopa/pizda/1aa.txt", vfs::creation_disposition::eCREATE_NEW, false);
    vfs::write(f, "1", 1);
    vfs::seek(f, 0, vfs::seek_type::eEND);
    sz = vfs::tell(f);
    EXPECT_EQ(1, sz);
    vfs::close(f);

    vfs::deinitialize();
}


TEST(OpenFileTest, CreateAlways) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");

    EXPECT_THROW(vfs::open("/zopa/pizda/1aa.txt", vfs::creation_disposition::eCREATE_ALWAYS, true), vfs::exception);

    vfs::file* f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eCREATE_ALWAYS, false);
    vfs::seek(f, 0, vfs::seek_type::eEND);
    auto sz = vfs::tell(f);
    EXPECT_EQ(0, sz);
    vfs::close(f);

    f = vfs::open("/zopa/pizda/1aa.txt", vfs::creation_disposition::eCREATE_ALWAYS, false);
    vfs::seek(f, 0, vfs::seek_type::eEND);
    sz = vfs::tell(f);
    EXPECT_EQ(0, sz);
    vfs::close(f);

    vfs::deinitialize();
}
