//
// Created by igor on 1/31/24.
//
#include <doctest/doctest.h>
#include <vfs/system.hh>
#include <vfs/stream.hh>
#include "utils/sandbox.hh"

TEST_CASE("stream read") {
	using namespace vfs;
	sandbox sbox;

	sbox.mkdir("zopa/pizda");
	sbox.create_file("zopa/pizda/1.txt", "sometestdata");

	mount("physfs", sbox.root(), "/");

	set_cwd ("/zopa/pizda");
	file_input_stream fis("1.txt");
	REQUIRE(fis.good());
	std::string read;
	fis >> read;
	REQUIRE(!read.empty());
}


TEST_CASE("stream write") {
	using namespace vfs;
	sandbox sbox;

	sbox.mkdir("zopa/pizda");

	mount("physfs", sbox.root(), "/");
	set_cwd ("/zopa/pizda");

	file_output_stream fos("1.txt");
	REQUIRE(fos.good());
	fos << "somedata";
	fos.close();

	file_input_stream fis("1.txt");
	REQUIRE(fis.good());
	std::string read;
	fis >> read;
	REQUIRE(!read.empty());
}

TEST_CASE("stream read/write") {
	using namespace vfs;
	sandbox sbox;

	sbox.mkdir("zopa/pizda");

	mount("physfs", sbox.root(), "/");
	set_cwd ("/zopa/pizda");

	file_output_stream fos("2.txt");
	REQUIRE(fos.good());
	fos << "somedata";
	fos.close();

	file_input_stream fis("2.txt");
	REQUIRE(fis.good());
	std::string read;
	fis >> read;
	REQUIRE(read == "somedata");
}

TEST_CASE("stream open") {
	using namespace vfs;
	sandbox sbox;

	sbox.mkdir("zopa/pizda");

	mount("physfs", sbox.root(), "/");
	set_cwd ("/zopa/pizda");

	file_output_stream ostr;
	ostr.open("test.txt", std::ios::out);
	REQUIRE (ostr.good());
	ostr.close();
}



TEST_CASE("stream seek") {
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

TEST_CASE("open out") {
	using namespace vfs;
	sandbox sbox;

	vfs::mount("physfs", sbox.root(), "/");

	vfs::file_output_stream ostr1("test.txt");
	ostr1 << "Hello, world!";
	ostr1.close();

	REQUIRE(sbox.exists ("test.txt"));
	REQUIRE(sbox.size ("test.txt") > 0);

	vfs::file_stream str1("test.txt");
	str1.close();

	REQUIRE(sbox.exists ("test.txt"));
	REQUIRE(sbox.size ("test.txt") > 0);

	vfs::file_output_stream ostr2("test.txt");
	ostr2.close();

	REQUIRE(sbox.exists ("test.txt"));
	REQUIRE(sbox.size ("test.txt") == 0);
}

TEST_CASE("open trunc") {
	using namespace vfs;
	sandbox sbox;

	vfs::mount ("physfs", sbox.root (), "/");
	vfs::file_output_stream ostr1("test.txt");
	ostr1 << "Hello, world!";
	ostr1.close();

	REQUIRE(sbox.exists ("test.txt"));
	REQUIRE(sbox.size ("test.txt") > 0);

	vfs::file_stream str1("test.txt", std::ios::trunc);
	str1.close();

	REQUIRE(sbox.exists ("test.txt"));
	REQUIRE(sbox.size ("test.txt") == 0);
}

TEST_CASE("open ate") {
	using namespace vfs;
	sandbox sbox;

	mount ("physfs", sbox.root (), "/");

	file_output_stream ostr("test.txt");
	ostr << "0123456789";
	ostr.close();

	file_stream str1("test.txt", std::ios::ate);
	int c = str1.get();
	REQUIRE (str1.eof());

	str1.clear();
	str1.seekg(0);
	c = str1.get();
	REQUIRE (c == '0');

	str1.close();

	file_stream str2("test.txt", std::ios::ate);
	str2 << "abcdef";
	str2.seekg(0);
	std::string s;
	str2 >> s;
	REQUIRE (s == "0123456789abcdef");
	str2.close();

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