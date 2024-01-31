//
// Created by igor on 1/31/24.
//
#include <doctest/doctest.h>
#include <vfs/system.hh>
#include <vfs/stream.hh>
#include "sandbox.hh"

TEST_CASE("seek") {
	using namespace vfs;
	sandbox sbox;

	vfs::mount("physfs", sbox.root(), "/");

	vfs::set_cwd ("/");

	file_stream str("test.txt", std::ios::out);
	str << "0123456789abcdef";

	str.seekg(0);
	int c = str.get();
	REQUIRE (c == '0');

	str.seekg(10);
	REQUIRE (str.tellg() == std::streampos(10));
	c = str.get();
	REQUIRE (c == 'a');
	REQUIRE (str.tellg() == std::streampos(11));

	str.seekg(-1, std::ios::end);
	REQUIRE (str.tellg() == std::streampos(15));
	c = str.get();
	REQUIRE (c == 'f');
	REQUIRE (str.tellg() == std::streampos(16));

	str.seekg(-1, std::ios::cur);
	REQUIRE (str.tellg() == std::streampos(15));
	c = str.get();
	REQUIRE (c == 'f');
	REQUIRE (str.tellg() == std::streampos(16));

	str.seekg(-4, std::ios::cur);
	REQUIRE (str.tellg() == std::streampos(12));
	c = str.get();
	REQUIRE (c == 'c');
	REQUIRE (str.tellg() == std::streampos(13));

	str.seekg(1, std::ios::cur);
	REQUIRE (str.tellg() == std::streampos(14));
	c = str.get();
	REQUIRE (c == 'e');
	REQUIRE (str.tellg() == std::streampos(15));

	vfs::deinitialize();
}

TEST_CASE("multi open") {
	using namespace vfs;
	sandbox sbox;

	vfs::mount("physfs", sbox.root(), "/");
	vfs::set_cwd ("/");

	file_stream str("test.txt", std::ios::out);
	str << "0123456789\n";
	str << "abcdefghij\n";
	str << "klmnopqrst\n";
	str.close();

	std::string s;
	str.open("test.txt", std::ios::in);
	std::getline(str, s);
	REQUIRE (s == "0123456789");
	str.close();

	str.open("test.txt", std::ios::in);
	std::getline(str, s);
	REQUIRE (s == "0123456789");
	str.close();
}

TEST_CASE("open app") {
	using namespace vfs;
	sandbox sbox;

	vfs::mount("physfs", sbox.root(), "/");

	vfs::set_cwd ("/");

	vfs::file_output_stream ostr("test.txt");
	REQUIRE_FALSE(!ostr);
	ostr << "0123456789";
	ostr.close();

	vfs::file_stream str1("test.txt", std::ios::app);
	REQUIRE_FALSE(!str1);

	str1 << "abc";

	str1.seekp(0);

	str1 << "def";

	str1.close();

	vfs::file_input_stream istr("/test.txt");
	std::string s;
	istr >> s;
	REQUIRE (s == "0123456789abcdef");
	istr.close();
}