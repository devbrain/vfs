#include "filesystem.hh"
#include <vfs/api/exception.hh>
#include <bsw/errors.hh>

namespace vfs::core
{
    filesystem::filesystem(vfs_module* ops)
            : _module(ops)
    {
    }
    // ---------------------------------------------------------------------
    filesystem::~filesystem()
    {
        _module->destructor(_module);
    }
    // ---------------------------------------------------------------------
    std::unique_ptr<inode> filesystem::load_root(const std::string& params)
    {
        return std::unique_ptr<inode>(new inode(_module->load_root(_module->opaque,
                                                                   const_cast<char*>(params.c_str())), this));
    }
    // ---------------------------------------------------------------------
    size_t filesystem::max_name_length() const noexcept
    {
        return _module->maximal_name_length(_module->opaque);
    }
    // ---------------------------------------------------------------------
    std::string filesystem::type() const noexcept
    {
        char name[128] = {0};
        _module->get_name(_module->opaque, name, sizeof(name));
        return name;
    }
    // ---------------------------------------------------------------------
    int filesystem::sync()
    {
        return _module->sync_filesystem(_module);
    }
    // ---------------------------------------------------------------------
    int filesystem::sync(inode* ino)
    {
        return _module->sync_inode(_module->opaque, ino->_ops->opaque);
    }
    // ======================================================================
    stats::stats()
            : vfs_inode_stats()
    {
        destructor = nullptr;

        num_of_additional_attributes = 0;
        additional_attributes = nullptr;
    }
    // ----------------------------------------------------------------------
    stats::~stats()
    {
        if (destructor)
        {
            destructor(this);
        }
    }
    // =====================================================================
    directory_iterator::~directory_iterator()
    {
        _ops->destructor(_ops);
    }
    // ----------------------------------------------------------------------
    bool directory_iterator::has_next() const
    {
        return _ops->has_next(_ops->opaque);
    }
    // ----------------------------------------------------------------------
    const inode* directory_iterator::owner() const
    {
        return _owner;
    }
    // ----------------------------------------------------------------------
    std::string directory_iterator::next()
    {
        std::string s;
        s.resize(_owner->owner()->max_name_length());
        size_t n = _ops->next(_ops->opaque, const_cast<char*>(s.c_str()), s.size());
        s.resize(n);
        return s;
    }
    // ----------------------------------------------------------------------
    directory_iterator::directory_iterator(vfs_directory_iterator* ops, const inode* owner)
            : _ops(ops), _owner(owner)
    {
    }
    // ========================================================================
    std::unique_ptr<inode> inode::lookup(const std::string& name) const
    {
        vfs_inode_ops* child = _ops->lookup(_ops->opaque, const_cast<char*>(name.c_str()));
        if (!child)
        {
            return nullptr;
        }
        return std::unique_ptr<inode>(new inode(child, _owner));
    }
    // ----------------------------------------------------------------------
    void inode::stat(stats& st) const
    {
        if (!_ops->stat(_ops->opaque, &st))
        {
            THROW_EXCEPTION_EX(vfs::exception, "failed to load stat");
        }
    }
    // ----------------------------------------------------------------------
    std::unique_ptr<directory_iterator> inode::get_directory_iterator() const
    {
        vfs_directory_iterator* itr = _ops->get_directory_iterator(_ops->opaque);
        if (!itr)
        {
            return nullptr;
        }
        return std::unique_ptr<directory_iterator>(new directory_iterator(itr, this));
    }
    // ----------------------------------------------------------------------
    std::unique_ptr<file_ops> inode::get_file_ops(open_mode_type mode_type) const
    {
        vfs_file_ops* ops = _ops->open_file(_ops->opaque, mode_type);
        if (!ops)
        {
            return nullptr;
        }
        return std::unique_ptr<file_ops>(new file_ops(ops, const_cast<inode*>(this)));
    }
    // ----------------------------------------------------------------------
    bool inode::mkdir(const std::string& name)
    {
        auto res = _ops->mkdir(_ops->opaque, const_cast<char*>(name.c_str())) == 1;
        if (res)
        {
            _make_dirty();
        }
        return res;
    }
    // ----------------------------------------------------------------------
    inode::inode(vfs_inode_ops* ops, filesystem* owner)
            : _ops(ops),
              _owner(owner),
              _dirty(false)
    {

    }
    // ----------------------------------------------------------------------
    inode::~inode()
    {
        if (_ops)
        {
            _ops->destructor(_ops);
        }
    }
    // ----------------------------------------------------------------------
    const filesystem* inode::owner() const
    {
        return _owner;
    }
    // ----------------------------------------------------------------------
    bool inode::dirty() const noexcept
    {
        return _dirty;
    }
    // ----------------------------------------------------------------------
    int inode::sync()
    {
        return _owner->sync(this);
    }
    // ----------------------------------------------------------------------
    int inode::unlink()
    {
        auto res = _ops->unlink(_ops->opaque);
        if (res)
        {
            _make_dirty();
        }
        return res;
    }
    // ----------------------------------------------------------------------
    void inode::_make_dirty()
    {
        _dirty = true;
    }
    // ========================================================================
    uint64_t file_ops::seek (uint64_t pos, enum whence_type whence)
    {
        return _ops->seek(_ops->opaque, pos, whence);
    }
    // ------------------------------------------------------------------------
    ssize_t file_ops::read (void* buff, size_t len)
    {
        return _ops->read(_ops->opaque, buff, len);
    }
    // ------------------------------------------------------------------------
    ssize_t file_ops::write (const void* buff, size_t len)
    {
        const ssize_t rc = _ops->write(_ops->opaque, const_cast<void*>(buff), len);
        if (rc > 0)
        {
            _owner->_make_dirty();
        }
        return rc;
    }
    // ------------------------------------------------------------------------
    bool file_ops::truncate()
    {
        if (_ops->truncate(_ops->opaque))
        {
            _owner->_make_dirty();
            return true;
        }
        return false;
    }
    // ------------------------------------------------------------------------
    file_ops::~file_ops()
    {
        _ops->destructor(_ops);
    }
    // ------------------------------------------------------------------------
    const inode* file_ops::owner() const
    {
        return _owner;
    }
    // ------------------------------------------------------------------------
    file_ops::file_ops (vfs_file_ops* ops, inode* owner)
    : _ops (ops),
    _owner(owner)
    {

    }
} // ns vfs