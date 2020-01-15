#include "sandbox.hh"
#include <fstream>
#include <iostream>

sandbox::sandbox()
{
    auto tmp = stdfs::temp_directory_path();
    _root = tmp / stdfs::path("unittest");

    if (stdfs::exists(_root))
    {
        stdfs::remove_all(_root);
    }
    stdfs::create_directories(_root);
}


void sandbox::mkdir(const std::string pth)
{
    auto path = _root / stdfs::path(pth);
    stdfs::create_directories(path);
}

void sandbox::create_file (const std::string pth, const std::string& text)
{
    auto path = _root / stdfs::path(pth);
    std::ofstream file(path); //open in constructor
    file << text;
}

std::string sandbox::root() const noexcept
{
    return _root.generic_string();
}