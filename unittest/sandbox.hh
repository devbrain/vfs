#ifndef UNITTEST_SANDBOX_HH
#define UNITTEST_SANDBOX_HH

#include <vfs/api/stdfilesystem.hh>

class sandbox
{
public:
    sandbox ();

    std::string root() const noexcept;

    void mkdir (const std::string pth);
    void create_file (const std::string path, const std::string& text);
private:
    stdfs::path _root;
};


#endif