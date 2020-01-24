#ifndef VFS_MOUNTS_HH
#define VFS_MOUNTS_HH

#include <vfs/api/vfs_api.h>
#include <vfs/api/stdfilesystem.hh>
#include <vfs/api/detail/wrapped_iterator.hh>
#include <memory>
#include <iosfwd>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning( disable : 4275 4251 )
#endif

namespace vfs
{
    namespace core
    {
        class fstab;
    }

    class VFS_API mounts;
    VFS_API mounts get_mounts();

    class VFS_API mounts
    {
        friend VFS_API mounts get_mounts();
        struct wrapper;
    public:
        class iterator;
    public:

        ~mounts ();
        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
    private:
        explicit mounts(core::fstab* mtable);
        struct impl;
        std::unique_ptr<impl> _impl;
    public:
		class data
		{
			friend class detail::iterator<data, wrapper>;
		private:
			std::string _path;
			std::string _type;
			std::string _args;
		public:

			[[nodiscard]] std::string type() const noexcept ;
			[[nodiscard]] std::string args() const noexcept ;
			[[nodiscard]] std::string path() const noexcept ;
		private:

			explicit data(const wrapper& itr);
		};
    public:
        class iterator : public detail::iterator<data, wrapper>
        {
            friend class fstab;
            friend class mounts;
        private:
            explicit iterator(wrapper& itr)
                    : detail::iterator<data, wrapper>(itr)
            {
            }
			bool _equals(const wrapper& a, const wrapper& b) const noexcept override;
			void _inc(wrapper& a) override;

        };
    };

    VFS_API [[nodiscard]] mounts::iterator begin(mounts& m);
    VFS_API [[nodiscard]] mounts::iterator end(mounts& m);

    VFS_API std::ostream& operator << (std::ostream&, const mounts::data&);
} // ns vfs

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif