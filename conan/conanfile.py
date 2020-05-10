from conans import ConanFile, CMake, tools
import os


class VFSConan(ConanFile):
    name = "vfs"
    version = "0.0.1"
    license = "<Put the package license here>"
    author = "Igor Gutnik"
    description = "VFS"
    settings = "os", "compiler", "build_type", "arch" 
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    generators = "cmake_find_package"
    requires = (
        "bsw/0.0.1@devbrain/stable",
        "doctest/2.3.4@bincrafters/stable"
    )
    def configure(self):
        if self.options.shared:
            self.options["bsw"].shared = True
    
    def _configure_cmake(self):
        cmake = CMake(self)
        if self.options.shared:
            cmake.definitions["BUILD_SHARED_LIBS"] = True
        else:
            cmake.definitions["BUILD_SHARED_LIBS"] = False
        cmake.definitions["ONYX_CONAN_FIND_PACKAGE"] = True
        cmake.configure(source_folder='vfs', build_folder='build')
        return cmake

    def source(self):
        self.run("git clone --recursive git@github.com:devbrain/vfs.git")
        cwd = os.getcwd()
        os.chdir(os.path.join(cwd, "vfs"))
        self.run("git submodule foreach git pull origin master")
        os.chdir(cwd)
    
    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["vfs"]


