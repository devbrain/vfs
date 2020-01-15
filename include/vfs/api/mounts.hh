#ifndef VFS_MOUNTS_HH
#define VFS_MOUNTS_HH

#include <vfs/api/vfs_api.h>
#include <vfs/api/stdfilesystem.hh>
#include <memory>
#include <iosfwd>

namespace vfs
{
    namespace core
    {
        class fstab;
    }


    class VFS_API mounts
    {
        friend mounts get_mounts();
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
        class iterator
        {
            friend class fstab;
            friend class mounts;
        public:
            class data
            {
                friend class iterator;
            private:
                std::string _path;
                std::string _type;
                stdfs::path _args;
            public:

                [[nodiscard]] std::string type() const noexcept ;
                [[nodiscard]] std::string args() const noexcept ;
                [[nodiscard]] std::string path() const noexcept ;
            private:

                explicit data(const wrapper& itr);
            };
        public:
            typedef data value_type;
            typedef std::ptrdiff_t difference_type;
            typedef data* pointer;
            typedef data& reference;
            typedef std::input_iterator_tag iterator_category;
        private:
            wrapper& _value;
            explicit iterator(wrapper& itr)
                    : _value(itr)
            {
            }
        public:
            [[nodiscard]] data operator*() const;

            bool operator==(const iterator& other) const;
            bool operator!=(const iterator& other) const;

            data operator++(int);
            iterator& operator++();

        };
    };

    VFS_API [[nodiscard]] mounts::iterator begin(mounts& m);
    VFS_API [[nodiscard]] mounts::iterator end(mounts& m);

    VFS_API std::ostream& operator << (std::ostream&, const mounts::iterator::data&);
} // ns vfs


#endif
