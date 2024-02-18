#include <doctest/doctest.h>
#include <vfs/system.hh>
#include <vfs/io.hh>
#include "vfs/exception.hh"
#include "utils/sandbox.hh"

TEST_CASE("openExisting") {
	sandbox sbox;
	sbox.mkdir ("zopa/pizda");
	sbox.create_file ("zopa/pizda/1.txt", "aaa");

	vfs::mount ("physfs", sbox.root (), "/");

	vfs::file* f = vfs::open ("/zopa/pizda/1.txt", vfs::creation_disposition::eOPEN_EXISTING, true);
	vfs::close (f);
}

TEST_CASE("openNonExisting") {
	sandbox sbox;
	sbox.mkdir ("zopa/pizda");
	sbox.create_file ("zopa/pizda/1.txt", "aaa");

	vfs::load_module (std::filesystem::path ("."));
	vfs::mount ("physfs", sbox.root (), "/");

	REQUIRE_THROWS_AS(vfs::open ("/zopa/pizda/for-read.txt", vfs::creation_disposition::eOPEN_EXISTING, true), vfs::exception);
	REQUIRE_THROWS_AS(vfs::open ("/zopa/pizda/for-write.txt", vfs::creation_disposition::eOPEN_EXISTING, false), vfs::exception);

	vfs::deinitialize ();
}

TEST_CASE("openTruncate") {
	sandbox sbox;
	sbox.mkdir ("zopa/pizda");
	sbox.create_file ("zopa/pizda/1.txt", "aaa");

	vfs::load_module (std::filesystem::path ("."));
	vfs::mount ("physfs", sbox.root (), "/");

	vfs::file* f = vfs::open ("/zopa/pizda/1.txt", vfs::creation_disposition::eOPEN_EXISTING, true);
	vfs::seek (f, 0, vfs::seek_type::eEND);
	auto sz = vfs::tell (f);
	REQUIRE(3 == sz);
	vfs::close (f);

	// throw here because readonly
	REQUIRE_THROWS_AS(vfs::open ("/zopa/pizda/1.txt", vfs::creation_disposition::eTRUNCATE_EXISTING, true), vfs::exception);

	f = vfs::open ("/zopa/pizda/1.txt", vfs::creation_disposition::eTRUNCATE_EXISTING, false);
	vfs::close (f);

	auto st = vfs::get_stats ("/zopa/pizda/1.txt");
	REQUIRE(st);
	REQUIRE(0 == st->size);
}

TEST_CASE("CreateNew") {
	sandbox sbox;
	sbox.mkdir ("zopa/pizda");
	sbox.create_file ("zopa/pizda/1.txt", "aaa");

	vfs::load_module (std::filesystem::path ("."));
	vfs::mount ("physfs", sbox.root (), "/");

	REQUIRE_THROWS_AS(vfs::open ("/zopa/pizda/1aa.txt", vfs::creation_disposition::eCREATE_NEW, true), vfs::exception);

	vfs::file* f = vfs::open ("/zopa/pizda/1.txt", vfs::creation_disposition::eCREATE_NEW, true);
	vfs::seek (f, 0, vfs::seek_type::eEND);
	auto sz = vfs::tell (f);
	REQUIRE(3 == sz);
	vfs::close (f);

	f = vfs::open ("/zopa/pizda/1aa.txt", vfs::creation_disposition::eCREATE_NEW, false);
	vfs::write (f, "1", 1);
	vfs::seek (f, 0, vfs::seek_type::eEND);
	sz = vfs::tell (f);
	REQUIRE(1 == sz);
	vfs::close (f);
}

TEST_CASE("CreateAlways") {
	sandbox sbox;
	sbox.mkdir ("zopa/pizda");
	sbox.create_file ("zopa/pizda/1.txt", "aaa");

	vfs::load_module (std::filesystem::path ("."));
	vfs::mount ("physfs", sbox.root (), "/");

	REQUIRE_THROWS_AS(vfs::open ("/zopa/pizda/1aa.txt", vfs::creation_disposition::eCREATE_ALWAYS, true), vfs::exception);

	vfs::file* f = vfs::open ("/zopa/pizda/1.txt", vfs::creation_disposition::eCREATE_ALWAYS, false);
	vfs::seek (f, 0, vfs::seek_type::eEND);
	auto sz = vfs::tell (f);
	REQUIRE(0 == sz);
	vfs::close (f);

	f = vfs::open ("/zopa/pizda/1aa.txt", vfs::creation_disposition::eCREATE_ALWAYS, false);
	vfs::seek (f, 0, vfs::seek_type::eEND);
	sz = vfs::tell (f);
	REQUIRE(0 == sz);
	vfs::close (f);
}
