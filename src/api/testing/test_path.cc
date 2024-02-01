//
// Created by igor on 2/1/24.
//

#include <doctest/doctest.h>
#include "api/detail/path.hh"

#include <stdexcept>




using namespace vfs;

TEST_SUITE("path test") {
	TEST_CASE("parse unix") {
		path p;
		p.parse ("", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "");

		p.parse ("/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/");

		p.parse ("/usr", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr");

		p.parse ("/usr/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/");

		p.parse ("usr/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/");

		p.parse ("usr", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p[0] == "usr");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr");

		p.parse ("/usr/local", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local");
	}

	TEST_CASE("parse unix 2") {
		path p;
		p.parse ("/usr/local/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/");

		p.parse ("usr/local/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local/");

		p.parse ("usr/local", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local");

		p.parse ("/usr/local/bin", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin");

		p.parse ("/usr/local/bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");
	}

	TEST_CASE("parse unix 3") {
		path p;
		p.parse ("//usr/local/bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr//local/bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local//bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local/bin//", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local/./bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("./usr/local/bin/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local/bin/");

		p.parse ("./usr/local/bin/./", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local/bin/");

		p.parse ("./usr/local/bin/.", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "usr/local/bin/.");
	}

	TEST_CASE("parse unix 4") {
		path p;
		p.parse ("/usr/local/lib/../bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local/lib/../bin/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin/");

		p.parse ("/usr/local/lib/../../", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/");

		p.parse ("/usr/local/lib/..", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "lib");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/lib/..");

		p.parse ("../usr/local/lib/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 4);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "usr");
		REQUIRE (p[2] == "local");
		REQUIRE (p[3] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "../usr/local/lib/");

		p.parse ("/usr/../lib/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/lib/");

		p.parse ("/usr/../../lib/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/lib/");

		p.parse ("local/../../lib/", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "../lib/");

		p.parse ("a/b/c/d", path::PATH_UNIX);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "a");
		REQUIRE (p[1] == "b");
		REQUIRE (p[2] == "c");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "a/b/c/d");
	}

	TEST_CASE("parse unix 5") {
		path p;
		p.parse ("/c:/windows/system32/", path::PATH_UNIX);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.get_device () == "c");
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "windows");
		REQUIRE (p[1] == "system32");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/c:/windows/system32/");
	}

	TEST_CASE("parse windows") {
		path p;
		p.parse ("", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "");

		p.parse ("/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\");

		p.parse ("\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\");

		p.parse ("/usr", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr");

		p.parse ("\\usr", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr");

		p.parse ("/usr/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\");

		p.parse ("\\usr\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\");
	}

	TEST_CASE("parse windows 2") {
		path p;
		p.parse ("usr/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\");

		p.parse ("usr", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p[0] == "usr");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr");

		p.parse ("usr\\", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\");

		p.parse ("/usr/local", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local");

		p.parse ("\\usr\\local", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local");

		p.parse ("/usr/local/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\");

		p.parse ("usr/local/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local\\");

		p.parse ("usr/local", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local");

		p.parse ("/usr/local/bin", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin");

		p.parse ("/usr/local/bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr//local/bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr/local//bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr/local/bin//", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");
	}

	TEST_CASE("parse windows 3") {
		path p;
		p.parse ("/usr/local/./bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("./usr/local/bin/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local\\bin\\");

		p.parse ("./usr/local/bin/./", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local\\bin\\");

		p.parse ("./usr/local/bin/.", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "usr\\local\\bin\\.");

		p.parse ("/usr/local/lib/../bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr/local/lib/../bin/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("\\usr\\local\\lib\\..\\bin\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\bin\\");

		p.parse ("/usr/local/lib/../../", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "usr");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\");

		p.parse ("/usr/local/lib/..", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "lib");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\usr\\local\\lib\\..");

		p.parse ("../usr/local/lib/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 4);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "usr");
		REQUIRE (p[2] == "local");
		REQUIRE (p[3] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "..\\usr\\local\\lib\\");

		p.parse ("/usr/../lib/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\lib\\");

		p.parse ("/usr/../../lib/", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\lib\\");

		p.parse ("local/../../lib/", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "lib");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "..\\lib\\");
	}

	TEST_CASE("parse windows 4") {
		path p;
		p.parse ("\\\\server\\files", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "files");
		REQUIRE (p.get_node () == "server");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\");

		p.parse ("\\\\server\\files\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "files");
		REQUIRE (p.get_node () == "server");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\");

		p.parse ("\\\\server\\files\\file", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "files");
		REQUIRE (p.get_node () == "server");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\file");

		p.parse ("\\\\server\\files\\dir\\file", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "files");
		REQUIRE (p[1] == "dir");
		REQUIRE (p.get_node () == "server");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\dir\\file");

		p.parse ("\\\\server\\files\\dir\\file", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "files");
		REQUIRE (p[1] == "dir");
		REQUIRE (p.get_node () == "server");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\dir\\file");

		p.parse ("\\\\server", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_node () == "server");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\");

		p.parse ("c:\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "c");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\");

		p.parse ("c:\\WinNT", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "c");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\WinNT");

		p.parse ("c:\\WinNT\\", path::PATH_WINDOWS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "WinNT");
		REQUIRE (p.get_device () == "c");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\WinNT\\");

		CHECK_THROWS_AS(p.parse ("~:\\", path::PATH_WINDOWS), std::exception);
		CHECK_THROWS_AS(p.parse ("c:file.txt", path::PATH_WINDOWS), std::exception);

		p.parse ("a\\b\\c\\d", path::PATH_WINDOWS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "a");
		REQUIRE (p[1] == "b");
		REQUIRE (p[2] == "c");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "a\\b\\c\\d");
	}

	TEST_CASE("parse vms") {
		path p;
		p.parse ("", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "");

		p.parse ("[]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "");

		p.parse ("[foo]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[.foo]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[.foo]");

		p.parse ("[foo.bar]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar]");

		p.parse ("[.foo.bar]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[.foo.bar]");

		p.parse ("[foo.bar.foobar]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p[2] == "foobar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar.foobar]");

		p.parse ("[.foo.bar.foobar]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p[2] == "foobar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[.foo.bar.foobar]");
	}

	TEST_CASE("parse vms2") {
		path p;
		p.parse ("[foo][bar]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar]");

		p.parse ("[foo.][bar]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar]");

		p.parse ("[foo.bar][foo]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p[2] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar.foo]");

		CHECK_THROWS_AS(p.parse ("[foo.bar][.foo]", path::PATH_VMS), std::exception);
		CHECK_THROWS_AS(p.parse ("[.foo.bar][foo]", path::PATH_VMS), std::exception);

		p.parse ("[-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[-]");

		p.parse ("[--]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[--]");

		p.parse ("[---]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");

		p.parse ("[-.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[--]");

		p.parse ("[.-.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[--]");

		p.parse ("[-.-.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");

		p.parse ("[.-.-.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");

		p.parse ("[.--.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");

		p.parse ("[--.-]", path::PATH_VMS);
		REQUIRE (p.is_relative ());
		REQUIRE (!p.is_absolute ());
		REQUIRE (p.depth () == 3);
		REQUIRE (p[0] == "..");
		REQUIRE (p[1] == "..");
		REQUIRE (p[2] == "..");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[---]");
	}

	TEST_CASE("parse vms 3") {
		path p;
		p.parse ("[foo][-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][--]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][-.-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][bar.-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][bar.foo.-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "foo");
		REQUIRE (p[1] == "bar");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo.bar]");

		p.parse ("[foo][bar.foo.--]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][bar.foo.---]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");

		p.parse ("[foo][bar.foo.-.-.-]", path::PATH_VMS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]");
	}

	TEST_CASE("parse guess") {
		path p;

		p.parse ("foo:bar.txt;5", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "foo");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());

		REQUIRE (p.to_string (path::PATH_VMS) == "foo:bar.txt;5");

		p.parse ("/usr/local/bin", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 2);
		REQUIRE (p[0] == "usr");
		REQUIRE (p[1] == "local");
		REQUIRE (p[2] == "bin");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/bin");

		p.parse ("\\\\server\\files", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "files");
		REQUIRE (p.get_node () == "server");
		REQUIRE (p.is_directory ());
		REQUIRE (!p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "\\\\server\\files\\");

		p.parse ("c:\\WinNT", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "c");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\WinNT");

		p.parse ("foo:bar.txt;5", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 0);
		REQUIRE (p.get_device () == "foo");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "foo:bar.txt;5");

		p.parse ("[foo]bar.txt", path::PATH_GUESS);
		REQUIRE (!p.is_relative ());
		REQUIRE (p.is_absolute ());
		REQUIRE (p.depth () == 1);
		REQUIRE (p[0] == "foo");
		REQUIRE (!p.is_directory ());
		REQUIRE (p.is_file ());
		REQUIRE (p.to_string (path::PATH_VMS) == "[foo]bar.txt");
	}

	TEST_CASE("test basename ext") {
		path p ("foo.bar");
		REQUIRE (p.get_file_name () == "foo.bar");
		REQUIRE (p.get_base_name () == "foo");
		REQUIRE (p.get_extension () == "bar");

		p.set_base_name ("readme");
		REQUIRE (p.get_file_name () == "readme.bar");
		REQUIRE (p.get_base_name () == "readme");
		REQUIRE (p.get_extension () == "bar");

		p.set_extension ("txt");
		REQUIRE (p.get_file_name () == "readme.txt");
		REQUIRE (p.get_base_name () == "readme");
		REQUIRE (p.get_extension () == "txt");

		p.set_extension ("html");
		REQUIRE (p.get_file_name () == "readme.html");
		REQUIRE (p.get_base_name () == "readme");
		REQUIRE (p.get_extension () == "html");

		p.set_base_name ("index");
		REQUIRE (p.get_file_name () == "index.html");
		REQUIRE (p.get_base_name () == "index");
		REQUIRE (p.get_extension () == "html");
	}

	TEST_CASE("test asolute") {
		path base ("C:\\Program Files\\", path::PATH_WINDOWS);
		path rel ("Poco");
		path abs = rel.absolute (base);
		REQUIRE (abs.to_string (path::PATH_WINDOWS) == "C:\\Program Files\\Poco");

		base.parse ("/usr/local", path::PATH_UNIX);
		rel.parse ("Poco/include", path::PATH_UNIX);
		abs = rel.absolute (base);
		REQUIRE (abs.to_string (path::PATH_UNIX) == "/usr/local/Poco/include");

		base.parse ("/usr/local/bin", path::PATH_UNIX);
		rel.parse ("../Poco/include", path::PATH_UNIX);
		abs = rel.absolute (base);
		REQUIRE (abs.to_string (path::PATH_UNIX) == "/usr/local/Poco/include");
	}

	TEST_CASE("test parent") {
		path p ("/usr/local/include", path::PATH_UNIX);
		p.make_parent ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/");
		p.make_parent ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/");
		p.make_parent ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "/");
		p.make_parent ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "/");
	}

	TEST_CASE("test directory") {
		path p = path ("/usr/local/include", path::PATH_UNIX);
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/include");

		p = path ("/usr/local/include/", path::PATH_UNIX);
		REQUIRE (p.to_string (path::PATH_UNIX) == "/usr/local/include/");
	}

	TEST_CASE("test swap") {
		path p1 ("c:\\temp\\foo.bar");
		path p2 ("\\\\server\\files\\foo.bar");
		p1.swap (p2);
		REQUIRE (p1.to_string () == "\\\\server\\files\\foo.bar");
		REQUIRE (p2.to_string () == "c:\\temp\\foo.bar");
	}

	TEST_CASE("test resolve") {
		path p ("c:\\foo\\", path::PATH_WINDOWS);
		p.resolve ("test.dat");
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\foo\\test.dat");

		p.assign ("c:\\foo\\", path::PATH_WINDOWS);
		p.resolve (path ("d:\\bar.txt", path::PATH_WINDOWS));
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "d:\\bar.txt");

		p.assign ("c:\\foo\\bar.txt", path::PATH_WINDOWS);
		p.resolve ("foo.txt");
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\foo\\foo.txt");

		p.assign (R"(c:\foo\bar\)", path::PATH_WINDOWS);
		p.resolve (path ("..\\baz\\test.dat", path::PATH_WINDOWS));
		REQUIRE (p.to_string (path::PATH_WINDOWS) == "c:\\foo\\baz\\test.dat");
	}

	TEST_CASE("test push pop") {
		path p;
		p.push_directory ("a");
		p.push_directory ("b");
		p.push_directory ("c");
		REQUIRE (p.to_string (path::PATH_UNIX) == "a/b/c/");

		p.pop_directory ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "a/b/");

		p.pop_front_directory ();
		REQUIRE (p.to_string (path::PATH_UNIX) == "b/");
	}
}
