#include <ostream>
#include <vfs/api/modules.hh>
#include "api/detail/modules_table.hh"


namespace vfs {

    struct modules::wrapper {
        wrapper(core::modules_table::map_t::const_iterator it)
                : itr(it) {

        }

        core::modules_table::map_t::const_iterator itr;
    };

    struct modules::impl {
        impl(core::modules_table::map_t::const_iterator beg, core::modules_table::map_t::const_iterator en)
                : begin(beg),
                  end(en) {

        }

        wrapper begin;
        wrapper end;
    };


    // ===================================================================================
    modules::modules(vfs::core::modules_table *mtable)
    {
        _impl = std::make_unique<impl>(mtable->_entries.begin(), mtable->_entries.end());
    }
    // ----------------------------------------------------------------------------------
    modules::~modules () = default;
    // ----------------------------------------------------------------------------------
    modules::iterator modules::begin() {
        return iterator(_impl->begin);
    }

    // ----------------------------------------------------------------------------------
    modules::iterator modules::end() {
        return iterator(_impl->end);
    }

    // ===================================================================================
    modules::iterator::data::data(const wrapper &w)
            : _type(w.itr->first), _refcount(w.itr->second->ref_count()), _path(w.itr->second->path()) {

    }

    // -----------------------------------------------------------------------------------
    std::string modules::iterator::data::type() const noexcept {
        return _type;
    }

    // -----------------------------------------------------------------------------------
    int modules::iterator::data::refcount() const noexcept {
        return _refcount;
    }
    // -----------------------------------------------------------------------------------
    stdfs::path modules::iterator::data::path() const noexcept {
        return _path;
    }

    // -----------------------------------------------------------------------------------
    modules::iterator::data modules::iterator::operator*() const {
        return data(_value);
    }

    // -----------------------------------------------------------------------------------
    bool modules::iterator::operator==(const iterator &other) const {
        return _value.itr == other._value.itr;
    }

    // -----------------------------------------------------------------------------------
    bool modules::iterator::operator!=(const iterator &other) const {
        return !(*this == other);
    }

    // -----------------------------------------------------------------------------------
    modules::iterator::data modules::iterator::operator++(int) {
        data ret(_value);
        (void) ++*this;
        return ret;
    }

    // -----------------------------------------------------------------------------------
    modules::iterator &modules::iterator::operator++() {
        ++_value.itr;
        return *this;
    }

    // ===================================================================================
    modules::iterator begin(modules &m) {
        return m.begin();
    }

    // ----------------------------------------------------------------------------------
    modules::iterator end(modules &m) {
        return m.end();
    }
    std::ostream& operator << (std::ostream& os, const modules::iterator::data& d)
    {
        os << d.type() << "\t" << d.path();
        return os;
    }
}