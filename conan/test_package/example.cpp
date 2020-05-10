#include <vfs/api/system.hh>
#include <vfs/api/exception.hh>


int main (int argc, char** argv)
{
    vfs::initialize();
    vfs::deinitialize();
    return 0;
}


