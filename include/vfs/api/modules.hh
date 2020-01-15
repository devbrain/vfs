#ifndef VFS_MODULES_HH
#define VFS_MODULES_HH

#include <vfs/api/vfs_api.h>
#include <vfs/api/stdfilesystem.hh>
#include <memory>
#include <iosfwd>

namespace vfs
{
    namespace core
    {
        class modules_table;
    }


    class VFS_API modules
    {
        friend modules get_modules();
        struct wrapper;
    public:
        class iterator;
    public:

        ~modules ();
        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
    private:
        explicit modules(core::modules_table* mtable);
        struct impl;
        std::unique_ptr<impl> _impl;
    public:
        class iterator
        {
            friend class modules_table;
            friend class modules;
        public:
            class data
            {
                friend class iterator;
            private:
                std::string _type;
                int _refcount;
                stdfs::path _path;
            public:
                [[nodiscard]] std::string type() const noexcept ;
                [[nodiscard]] int refcount() const noexcept ;
                [[nodiscard]] stdfs::path path() const noexcept ;
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

    VFS_API [[nodiscard]] modules::iterator begin(modules& m);
    VFS_API [[nodiscard]] modules::iterator end(modules& m);

    VFS_API std::ostream& operator << (std::ostream&, const modules::iterator::data&);
} // ns vfs


#endif
