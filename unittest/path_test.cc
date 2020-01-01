#include <gtest/gtest.h>
#include <vfs/api/path.hh>

TEST(PathTest, Parse1) {
	vfs::path p;

	p.parse("/");
	
	EXPECT_TRUE (p.depth() == 0);
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/");

	p.parse("/usr");

	EXPECT_TRUE (p.depth() == 0);
	EXPECT_TRUE (!p.is_directory());
	EXPECT_TRUE (p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr");

	p.parse("/usr/");
	EXPECT_TRUE (p.depth() == 1);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/");


	p.parse("/usr/local");
	EXPECT_TRUE (p.depth() == 1);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (!p.is_directory());
	EXPECT_TRUE (p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local");

}

TEST(PathTest, Parse2) {
	vfs::path p;

	p.parse("/usr/local/");
	EXPECT_TRUE (p.depth() == 2);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/");


	p.parse("/usr/local/bin");
	EXPECT_TRUE (p.depth() == 2);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (!p.is_directory());
	EXPECT_TRUE (p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin");

	p.parse("/usr/local/bin/");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");
}

TEST(PathTest, Parse3) {
	vfs::path p;

	p.parse("//usr/local/bin/" );
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");

	p.parse("/usr//local/bin/");

	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");

	p.parse("/usr/local//bin/");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");

	p.parse("/usr/local/bin//");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");

	p.parse("/usr/local/./bin/");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");


	p.parse("/usr/local/bin/./");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");

}

TEST(PathTest, Parse4) {
	vfs::path p;
	p.parse("/usr/local/lib/../bin/");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");

	p.parse("/usr/local/lib/../bin/");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "bin");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/bin/");

	p.parse("/usr/local/lib/../../");
	EXPECT_TRUE (p.depth() == 1);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/");

	p.parse("/usr/local/lib/..");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "usr");
	EXPECT_TRUE (p[1] == "local");
	EXPECT_TRUE (p[2] == "lib");
	EXPECT_TRUE (!p.is_directory());
	EXPECT_TRUE (p.is_file());
	EXPECT_TRUE (p.to_string() == "/usr/local/lib/..");



	p.parse("/usr/../lib/");
	EXPECT_TRUE (p.depth() == 1);
	EXPECT_TRUE (p[0] == "lib");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/lib/");

	p.parse("/usr/../../lib/");
	EXPECT_TRUE (p.depth() == 1);
	EXPECT_TRUE (p[0] == "lib");
	EXPECT_TRUE (p.is_directory());
	EXPECT_TRUE (!p.is_file());
	EXPECT_TRUE (p.to_string() == "/lib/");



	p.parse("/a/b/c/d");
	EXPECT_TRUE (p.depth() == 3);
	EXPECT_TRUE (p[0] == "a");
	EXPECT_TRUE (p[1] == "b");
	EXPECT_TRUE (p[2] == "c");
	EXPECT_TRUE (!p.is_directory());
	EXPECT_TRUE (p.is_file());
	EXPECT_TRUE (p.to_string() == "/a/b/c/d");
}

TEST(PathTest, BaseName) {
	vfs::path p("foo.bar");

	EXPECT_TRUE (p.get_file_name() == "foo.bar");
	EXPECT_TRUE (p.get_base_name() == "foo");
	EXPECT_TRUE (p.get_extension() == "bar");

	p.set_base_name("readme");
	EXPECT_TRUE (p.get_file_name() == "readme.bar");
	EXPECT_TRUE (p.get_base_name() == "readme");
	EXPECT_TRUE (p.get_extension() == "bar");

	p.set_extension("txt");
	EXPECT_TRUE (p.get_file_name() == "readme.txt");
	EXPECT_TRUE (p.get_base_name() == "readme");
	EXPECT_TRUE (p.get_extension() == "txt");

	p.set_extension("html");
	EXPECT_TRUE (p.get_file_name() == "readme.html");
	EXPECT_TRUE (p.get_base_name() == "readme");
	EXPECT_TRUE (p.get_extension() == "html");

	p.set_base_name("index");
	EXPECT_TRUE (p.get_file_name() == "index.html");
	EXPECT_TRUE (p.get_base_name() == "index");
	EXPECT_TRUE (p.get_extension() == "html");
}

TEST(PathTest, Parent)
{
	vfs::path p("/usr/local/include");
	p.make_parent();
	EXPECT_TRUE (p.to_string() == "/usr/local/");
	p.make_parent();
	EXPECT_TRUE (p.to_string() == "/usr/");
	p.make_parent();
	EXPECT_TRUE (p.to_string() == "/");
	p.make_parent();
	EXPECT_TRUE (p.to_string() == "/");
}

TEST(PathTest, Swap)
{
	const std::string s1("/usr/local/include");
	const std::string s2("/foo.bar");

	vfs::path p1(s1);
	vfs::path p2(s2);
	vfs::swap(p1, p2);
	EXPECT_TRUE(p1.to_string() == s2);
	EXPECT_TRUE(p2.to_string() == s1);
}