#include <vector>

#include <gtest/gtest.h>
#include <vfs/api/system.hh>
#include <vfs/api/exception.hh>

#include "sandbox.hh"

TEST(ReadWriteTest, SimpleRead) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    const char* txt = "123";
    sbox.create_file("zopa/pizda/1.txt", txt);


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");

    vfs::file* f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eOPEN_EXISTING, true);
    vfs::seek(f, 0, vfs::seek_type::eEND);
    auto sz = vfs::tell(f);
    EXPECT_EQ(sz, 3);
    vfs::seek(f, 0, vfs::seek_type::eSET);
    std::vector<char> temp;
    temp.resize(sz);
    auto rc = vfs::read(f, temp.data(), sz);
    EXPECT_EQ(rc, sz);

    EXPECT_EQ(strncmp(temp.data(), txt, sz), 0);
    vfs::close(f);

    vfs::deinitialize();
}
// --------------------------------------------------------------------
TEST(ReadWriteTest, SimpleReadSeek) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    const char* txt = "123";
    sbox.create_file("zopa/pizda/1.txt", txt);


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");

    vfs::file* f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eOPEN_EXISTING, true);
    vfs::seek(f, 0, vfs::seek_type::eEND);
    auto sz = vfs::tell(f);
    EXPECT_EQ(sz, 3);
    vfs::seek(f, 1, vfs::seek_type::eSET);
    std::vector<char> temp;
    temp.resize(sz);
    auto rc = vfs::read(f, temp.data(), sz);
    EXPECT_EQ(rc, 2);

    EXPECT_EQ(strncmp(temp.data(), txt+1, rc), 0);
    vfs::close(f);

    vfs::deinitialize();
}

TEST(ReadWriteTest, WriteFailed) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    const char* txt = "123";
    sbox.create_file("zopa/pizda/1.txt", txt);


    vfs::load_module(stdfs::path("."));
    vfs::mount("physfs", sbox.root(), "/");

    vfs::file* f = vfs::open("/zopa/pizda/1.txt", vfs::creation_disposition::eOPEN_EXISTING, true);
    EXPECT_THROW(vfs::write(f, txt, 1), vfs::exception);
    vfs::close(f);

    vfs::deinitialize();
}
