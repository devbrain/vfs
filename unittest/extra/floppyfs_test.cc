//
// Created by igor on 3/10/24.
//

#include <doctest/doctest.h>
#include <vfs/extra/floppyfs.hh>
#include <vfs/archive.hh>
#include <vector>
#include <tuple>
#include "utils/test_data.hh"

TEST_SUITE("floppyfs test") {
	TEST_CASE("Read floppy dirs") {
		vfs::archive floppy (vfs::extra::create_floppyfs (), get_test_file ("floppyfs/Disk.img"));

		static auto names = std::vector<std::tuple<std::string, uint64_t>>{
			{"bootsect.h", 3089},
			{"bpb.h", 7423},
			{"CMakeLists.txt", 174},
			{"direntry.h", 4580},
			{"dispsig.cpp", 5221},
			{"dispsig.mak", 226},
			{"dispsrch.txt", 7975},
			{"dos.c", 6736},
			{"dos_cp", 33904},
			{"dos_cp.c", 12765},
			{"dos_cp.o", 21360},
			{"dos.h", 627},
			{"dos_ls", 26120},
			{"dos_ls.c", 2656},
			{"dos_ls.o", 10576},
			{"dos.o", 14224},
			{"fat.h", 3147},
			{"Makefile", 196},
			{"README.md", 2568},
			{"srchsig.cpp", 6413},
			{"srchsig.mak", 295}
		};
		auto d = floppy.open_directory ("/");
		while (d.has_next()) {
			const auto [name, st] = d.next();
			if (st.type == vfs::stats::eDIRECTORY) {
				REQUIRE(name == "1");
			} else {
				bool  found = false;
				for (const auto& [exp_name, exp_size] : names) {
					if (exp_name == name) {
						found = true;
						REQUIRE(st.type == vfs::stats::eFILE);
						REQUIRE(st.size == exp_size);
						break;
					}
				}
				REQUIRE(found);
			}
		}
	}
}