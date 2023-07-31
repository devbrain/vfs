//
// Created by igor on 7/19/23.
//

#include <stdexcept>
#include <iostream>
#include <vfs/vfs.hh>

int main(int args, char* argv[]) {
  try {
    vfs::auto_init vfs_initializer;
    std::cout << "VFS initialized" << std::endl;
    std::cout << "Registered modules" << std::endl;
    vfs::enumerate_registered_modules ([](auto& md) {
      std::cout << "\t" << md.name;
      if (md.so_path) {
        std::cout << " @ " << md.so_path->u8string();
      }
      std::cout << " ARGS: " << md.help;
      std::cout << std::endl;
    });
    vfs::mount (vfs::path("/"), "physfs", "/tmp --read-only");
    auto s = vfs::get_stat (vfs::path("/tmpaddon"));
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}
