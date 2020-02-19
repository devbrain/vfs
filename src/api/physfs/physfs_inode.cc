//
// Created by igor on 02/01/2020.
//

#include <stdio.h>
#if !defined(WIN32)
#include <unistd.h>
#else
#include <io.h>
#endif

#include <bsw/macros.hh>
#include "physfs_inode.hh"

#if defined(WIN32)
#define fopen _wfopen
#define SLIT(X) CONCATENATE(L, X)
#else
#define SLIT(X) X
#endif

namespace vfs::detail
{

    class file : public vfs::module::file
    {
    public:
        explicit file(FILE* f);
    private:
        ~file() override;
        ssize_t read(void* buff, size_t len) override;
        ssize_t write(void* buff, size_t len) override;
        bool seek(uint64_t pos, enum whence_type whence) override;
        bool truncate() override;
        uint64_t tell() const override;
    private:
        FILE* _file;
    };

    class directory_iterator : public vfs::module::directory_iterator
    {
    public:
        directory_iterator(stdfs::directory_iterator di)
                : _di(di),
                  _itr(begin(_di)),
                  _end(end(_di))
        {

        }

        bool has_next() override
        {
            return _itr != _end;
        }

        std::string next() override
        {
            std::string ret = _itr->path().filename().string();
            _itr++;
            return ret;
        }
    private:
        stdfs::directory_iterator _di;
        stdfs::directory_iterator _itr;
        stdfs::directory_iterator _end;
    };

// ===================================================================================
    physfs_inode::physfs_inode(const stdfs::path& pth)
            : vfs::module::inode(stdfs::is_directory(pth) ? VFS_INODE_DIRECTORY : VFS_INODE_REGULAR),
              _path(pth)
    {

    }
// -----------------------------------------------------------------------------------
    vfs::module::inode* physfs_inode::lookup(const char* name)
    {
        const stdfs::path child = _path / name;

        if (!stdfs::exists(child))
        {
            return nullptr;
        }
        return new physfs_inode(child);
    }
// -----------------------------------------------------------------------------------
    vfs::module::directory_iterator* physfs_inode::get_directory_iterator()
    {
        return new directory_iterator(stdfs::directory_iterator(_path));
    }
// -----------------------------------------------------------------------------------
    uint64_t physfs_inode::size()
    {
        if (VFS_INODE_DIRECTORY == type())
        {
            return 0;
        }
        return stdfs::file_size(_path);
    }
// -----------------------------------------------------------------------------------
    bool physfs_inode::mkdir(const char* name)
    {
        const stdfs::path child = _path / name;
        if (stdfs::exists(child))
        {
            return false;
        }
        return stdfs::create_directory(child);
    }
// -----------------------------------------------------------------------------------
    bool physfs_inode::mkfile(const char* name)
    {
        const stdfs::path child = _path / name;
        if (stdfs::exists(child))
        {
            return false;
        }
        FILE* f = fopen(child.c_str(), SLIT("wb"));
        if (!f)
        {
            return false;
        }
        fclose(f);
        return true;
    }
// -----------------------------------------------------------------------------------
    int physfs_inode::unlink()
    {
        if (!stdfs::exists(_path))
        {
            return 0;
        }
        return stdfs::remove(_path) ? 1 : 0;
    }
// -----------------------------------------------------------------------------------
    vfs::module::file* physfs_inode::open_file(open_mode_type mode_type)
    {
        FILE* f = nullptr;

        if (mode_type == eVFS_OPEN_MODE_READ)
        {
            f = fopen(_path.c_str(), SLIT("rb"));
        } else
        {
            f = fopen(_path.c_str(), SLIT("rb+"));
        }
        if (!f)
        {
            return nullptr;
        }
        return new file(f);
    }
// ===========================================================================================
    file::file(FILE* f)
            : _file(f)
    {

    }
// -----------------------------------------------------------------------------------
    file::~file()
    {
        fclose(_file);
    }
// -----------------------------------------------------------------------------------
    ssize_t file::read(void* buff, size_t len)
    {

        return fread(buff, 1, len, _file);

    }
// -----------------------------------------------------------------------------------
    ssize_t file::write(void* buff, size_t len)
    {
        auto rc = fwrite(buff, 1, len, _file);
        return rc;
    }
// -----------------------------------------------------------------------------------
    bool file::seek(uint64_t pos, enum whence_type whence)
    {
        int w = SEEK_SET;
        switch (whence)
        {
            case eVFS_SEEK_SET:
                w = SEEK_SET;
                break;
            case eVFS_SEEK_END:
                w = SEEK_END;
                break;
            case eVFS_SEEK_CUR:
                w = SEEK_CUR;
                break;
        }
#if defined(WIN32)
#define fseek _fseeki64
#endif
        return fseek(_file, pos, w) == 0;
    }
// -----------------------------------------------------------------------------------
    bool file::truncate()
    {
#if !defined(WIN32)
        return ftruncate(fileno(_file), 0) == 0;
#else
        return _chsize(fileno(_file), 0) == 0;
#endif
    }
// -----------------------------------------------------------------------------------
    uint64_t file::tell() const
    {
        return ftell(_file);
    }
}