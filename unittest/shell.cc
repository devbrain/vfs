//
// Created by igor on 26/12/2019.
//

#include <stdexcept>
#include <iostream>
#include "vfs/api/system.hh"

int main (int argc, char* argv[])
{
	try
	{
		vfs::load_module(argv[1]);
		vfs::mount("physfs", "/home/igor", "/");
		vfs::unmount("/");
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
