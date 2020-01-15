#include <ostream>
#include <vfs/api/mounts.hh>
#include "api/detail/fstab.hh"


namespace vfs {

    struct mounts::wrapper {
        wrapper(core::fstab::entry_map_t::const_iterator it)
                : itr(it) {

        }

        core::fstab::entry_map_t::const_iterator itr;
    };

    struct mounts::impl {
        impl(core::fstab::entry_map_t::const_iterator beg, core::fstab::entry_map_t::const_iterator en)
                : begin(beg),
                  end(en) {

        }

        wrapper begin;
        wrapper end;
    };


    // ===================================================================================
    mounts::mounts(vfs::core::fstab *mtable)
    {
        _impl = std::make_unique<impl>(mtable->_fstab.begin(), mtable->_fstab.end());
    }
    // ----------------------------------------------------------------------------------
    mounts::~mounts () = default;
    // ----------------------------------------------------------------------------------
    mounts::iterator mounts::begin() {
        return iterator(_impl->begin);
    }

    // ----------------------------------------------------------------------------------
    mounts::iterator mounts::end() {
        return iterator(_impl->end);
    }

    // ===================================================================================
    mounts::iterator::data::data(const wrapper &w)
            : _path(w.itr->second.mount_path().to_string()), _type(w.itr->second.type()), _args(w.itr->second.args()) {

    }

    // -----------------------------------------------------------------------------------
    std::string mounts::iterator::data::type() const noexcept {
        return _type;
    }

    // -----------------------------------------------------------------------------------
    std::string mounts::iterator::data::args() const noexcept {
        return _args;
    }
    // -----------------------------------------------------------------------------------
    std::string mounts::iterator::data::path() const noexcept {
        return _path;
    }

    // -----------------------------------------------------------------------------------
    mounts::iterator::data mounts::iterator::operator*() const {
        return data(_value);
    }

    // -----------------------------------------------------------------------------------
    bool mounts::iterator::operator==(const iterator &other) const {
        return _value.itr == other._value.itr;
    }

    // -----------------------------------------------------------------------------------
    bool mounts::iterator::operator!=(const iterator &other) const {
        return !(*this == other);
    }

    // -----------------------------------------------------------------------------------
    mounts::iterator::data mounts::iterator::operator++(int) {
        data ret(_value);
        (void) ++*this;
        return ret;
    }

    // -----------------------------------------------------------------------------------
    mounts::iterator &mounts::iterator::operator++() {
        ++_value.itr;
        return *this;
    }

    // ===================================================================================
    mounts::iterator begin(mounts &m) {
        return m.begin();
    }

    // ----------------------------------------------------------------------------------
    mounts::iterator end(mounts &m) {
        return m.end();
    }
    std::ostream& operator << (std::ostream& os, const mounts::iterator::data& d)
    {
        os << d.type() << "\t" << d.path() << "\t" << d.args();
        return os;
    }
}