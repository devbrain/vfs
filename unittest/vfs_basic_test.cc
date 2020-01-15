#include <gtest/gtest.h>
#include <vfs/api/system.hh>
#include "sandbox.hh"


TEST(BasicTest, testMount) {
    sandbox sbox;
    sbox.mkdir("zopa/pizda");
    sbox.create_file("zopa/pizda/1.txt", "aaa");


    vfs::load_module(stdfs::path("."));

    for (const auto& x : vfs::get_modules())
    {
        std::cout << x << std::endl;
    }

    vfs::mount("physfs", sbox.root(), "/");

    for (const auto& x : vfs::get_mounts())
    {
        std::cout << x << std::endl;
    }

}


