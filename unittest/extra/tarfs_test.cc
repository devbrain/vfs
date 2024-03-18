//
// Created by igor on 3/10/24.
//

#include <vector>
#include <tuple>
#include <doctest/doctest.h>

#include <bsw/io/memory_stream_buf.hh>

#include <vfs/extra/tarfs.hh>
#include <vfs/archive.hh>

#include "utils/test_data.hh"
#include "utils/sha1.hh"

TEST_SUITE("tarfs test") {
	TEST_CASE("Read tar dirs") {
		vfs::archive tar (vfs::extra::create_tarfs (), get_test_file ("tarfs/disk.tar"));

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
		auto d = tar.open_directory ("/");
		while (d.has_next ()) {
			const auto [name, st] = d.next ();
			if (st.type == vfs::stats::eDIRECTORY) {
				REQUIRE(name == "1");
			} else {
				bool found = false;
				for (const auto& [exp_name, exp_size] : names) {
					if (exp_name == name) {
						found = true;
						REQUIRE(st.type == vfs::stats::eFILE);
						REQUIRE(st.size == exp_size);
						break;
					}
				}
				if (!found) {
					REQUIRE(found);
				}
			}
		}
	}

	TEST_CASE("read whole file") {
		static auto expected = std::vector<std::tuple<std::string, std::string>>{
			{"cafd246d24db5ee9ea33fda5acb7dfd5cfcdf755", "/README.md"},
			{"d18f743b9557ef90e89041556062ebf760120b52", "/Makefile"},
			{"746a0a1d1c20d284ad92888991fe7edd415f52bb", "/bootsect.h"},
			{"e5c6af9e6a846206959858afc851912105864dd9", "/bpb.h"},
			{"a5a47dd0bd4d2ea552722f803a74c465860fc275", "/direntry.h"},
			{"46899025f0a410cb5b707876c2fb991f21fb7b79", "/dos.c"},
			{"99b7513bd6e131171ec1df748ded79593908a928", "/dos.h"},
			{"6c5b862fe4934be64a1f95cbd6415c37cb6bfeb1", "/dos.o"},
			{"d98fa503a8cb5022e7914cce2ff13dd353bdf9db", "/dos_cp"},
			{"797d4b9a35d1fadee8d11e9e608f9a5e9367a5f5", "/dos_cp.c"},
			{"cdb11837c586954cac4c9c05ff6df24f83cfa8b5", "/dos_cp.o"},
			{"f9d2021c9b1403fe5fdcf0122ae078a331525d4a", "/dos_ls"},
			{"497baf39a1c912ebff589226b0a2dde59c4d597f", "/dos_ls.c"},
			{"24b18435c49d424053e786c9f30ba652bbc22a49", "/dos_ls.o"},
			{"e156065bd94cf15b22f2a75776612be3de9c51b3", "/fat.h"},
			{"5841e0e80f770611509b640e150709f10842e79d", "/CMakeLists.txt"},
			{"639e9822565bd0311abd7f8b26e32260422c1aec", "/dispsig.cpp"},
			{"ef2d911ccee24553f4a99ffdfe31958a9d31453a", "/dispsig.mak"},
			{"3aad7a33ee9e07248826884a2640158617ea080b", "/dispsrch.txt"},
			{"b2907314db9850fa491b0ad8c45330f4a403591a", "/srchsig.cpp"},
			{"9e3fee5b164a34995da5a14c2906b328369b8135", "/srchsig.mak"},
			{"5c24186326cf2a4e49b7bfcd4927dc8aba46a938", "/1/LIB/BWCC.LIB"},
			{"7efb56d4537740037774d84c4ee6fce1a10515fe", "/1/LIB/C0C.OBJ"},
			{"d05918d8b73723cc50a157c36c169631cf862ca4", "/1/LIB/2/C0.ASM"}
		};

		vfs::archive tar (vfs::extra::create_tarfs (), get_test_file ("tarfs/disk.tar"));
		for (const auto& [exp_sha1, name] : expected) {
			auto st = tar.get_stats (name);
			REQUIRE(st.has_value ());
			REQUIRE(st->type == vfs::stats::eFILE);
			auto* f = tar.open (name, vfs::READ_ONLY);
			REQUIRE(f);
			auto size = st->size;
			char buff[1024];
			std::size_t has_bytes = 0;
			SHA1 hasher;
			while (has_bytes < size) {
				auto rc = vfs::read (f, buff, sizeof (buff));
				REQUIRE(rc >= 0);
				has_bytes += rc;
				bsw::io::memory_input_stream mis (buff, rc);
				hasher.update (mis);
			}
			vfs::close (f);
			auto chks_sum = hasher.final ();

			REQUIRE(exp_sha1 == chks_sum);

		}
	}

	TEST_CASE("One byte read") {
		vfs::archive tar (vfs::extra::create_tarfs (), get_test_file ("tarfs/disk.tar"));
		auto st = tar.get_stats ("/README.md");
		REQUIRE(st.has_value ());
		REQUIRE(st->type == vfs::stats::eFILE);
		auto* f = tar.open ("/README.md", vfs::READ_ONLY);
		REQUIRE(f);
		auto size = st->size;
		std::size_t has_bytes = 0;
		SHA1 hasher;
		while (has_bytes < size) {
			char ch;
			auto rc = vfs::read (f, &ch, 1);
			REQUIRE(rc >= 0);
			has_bytes += rc;
			std::string s;
			s += ch;
			hasher.update (s);
		}
		vfs::close (f);
		auto chks_sum = hasher.final ();
		REQUIRE("cafd246d24db5ee9ea33fda5acb7dfd5cfcdf755" == chks_sum);
	}
}