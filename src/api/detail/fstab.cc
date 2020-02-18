#include "fstab.hh"
#include <vfs/api/exception.hh>
#include <bsw/errors.hh>
#include <bsw/logger/logger.hh>



namespace
{
    // http://jguegant.github.io/blogs/tech/performing-try-emplace.html
    template<class Factory>
    struct lazy_convert_construct
    {
        using result_type = std::invoke_result_t<const Factory&>; // Use some traits to check what would be the return type of the lambda if called.

        constexpr lazy_convert_construct(Factory&& factory)
                : factory_(std::move(factory)) // Let's store the factory for a latter usage.
        {
        }

        //                                     ↓ Respect the same nowthrow properties as the lambda factory.
        constexpr operator result_type() const noexcept(std::is_nothrow_invocable_v<const Factory&>)
        //        ^ enable       ^ the type this struct can be converted to
        //          conversion
        {
            return factory_();  // Delegate the conversion job to the lambda factory.
        }

        Factory factory_;
    };

}

namespace vfs::core
{
    fstab::entry::entry(filesystem* fs, const path& pth, const std::string& args)
            : _type(fs->type()),
              _args(args),
              _path(pth),
              _fs(fs)
    {
        _mount_point = std::make_unique<mount_point>(fs->load_root(args));
    }
    // -------------------------------------------------------------------------------------
    fstab::entry::~entry()
    {
        int err = _fs->sync();
        if (err != 1)
        {
            EVLOG_TRACE(EVLOG_ERROR, "Failed to sync FS ", _fs->type());
        }
    }
    // -------------------------------------------------------------------------------------
    path fstab::entry::mount_path() const noexcept
    {
        return _path;
    }
    // -------------------------------------------------------------------------------------
    std::string fstab::entry::args() const noexcept
    {
        return _args;
    }
    // -------------------------------------------------------------------------------------
    std::string fstab::entry::type() const noexcept
    {
        return _type;
    }
    bool fstab::entry::is_readonly() const noexcept
    {
        return _fs->is_readonly();
    }
    // -------------------------------------------------------------------------------------
    wrapped_pointer<mount_point> fstab::entry::get() const
    {
        return wrapped_pointer<mount_point>(_mount_point.get());
    }
    // =====================================================================================
    wrapped_pointer<mount_point> fstab::mount(filesystem* module, const path& mount_path, const std::string& args)
    {
        auto key = mount_path.hash();
        auto[itr, result] = _fstab.try_emplace(key,
                                               lazy_convert_construct([&] { return entry(module, mount_path, args); }));
        if (!result)
        {
            THROW_EXCEPTION_EX(vfs::exception, "this path is already mounted");
        }
        return itr->second.get();
    }
    // ------------------------------------------------------------------------------------
    void fstab::unmount(const path& pth)
    {
        auto key = pth.hash();
        auto itr = _fstab.find(key);

        if (itr == _fstab.end())
        {
            THROW_EXCEPTION_EX(vfs::exception, "no mount point found");
        }
        _fstab.erase(itr);
    }
} // ns vfs::core
