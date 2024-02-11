#include "file_system.hh"
#include "vfs/exception.hh"
#include <bsw/exception.hh>
#include <vector>

namespace vfs::core {
	file_system::file_system (vfs_module* ops)
		: _module (ops) {
		_is_readonly = _module->is_readonly (_module->opaque);
	}

	// ---------------------------------------------------------------------
	file_system::~file_system () {
		_module->destructor (_module);
	}

	// ---------------------------------------------------------------------
	std::unique_ptr<inode> file_system::load_root (const std::string& params) {
		return std::unique_ptr<inode> (new inode (_module->load_root (_module->opaque,
																	  const_cast<char*>(params.c_str ())), this));
	}

	// ---------------------------------------------------------------------
	size_t file_system::max_name_length () const noexcept {
		return _module->maximal_name_length (_module->opaque);
	}

	// ---------------------------------------------------------------------
	std::string file_system::type () const noexcept {
		char name[128] = {0};
		_module->get_name (_module->opaque, name, sizeof (name));
		return name;
	}

	// ---------------------------------------------------------------------
	std::string file_system::description () const noexcept {
		std::size_t n = 1024;
		std::vector<char> out (n, '\0');
		while (true) {
			auto rc = _module->get_description (_module->opaque, out.data (), out.size ());
			if (rc < out.size ()) {
				break;
			}
			n += 128;
			out.resize (n, '\0');
		}
		return {out.data ()};
	}

	// ---------------------------------------------------------------------
	bool file_system::is_readonly () const {
		return _is_readonly;
	}

	int file_system::sync () {
		if (!_is_readonly) {
			return _module->sync_filesystem (_module);
		}
		return 1;
	}

	// ---------------------------------------------------------------------
	int file_system::sync (inode* ino) {
		if (!_is_readonly) {
			return _module->sync_inode (_module->opaque, ino->_ops->opaque);
		}
		return 1;
	}

	// ======================================================================
	stats::stats ()
		: vfs_inode_stats () {
		destructor = nullptr;

		num_of_additional_attributes = 0;
		additional_attributes = nullptr;
	}

	// ----------------------------------------------------------------------
	stats::~stats () {
		if (destructor) {
			destructor (this);
		}
	}

	// =====================================================================
	directory_iterator::~directory_iterator () {
		_ops->destructor (_ops);
	}

	// ----------------------------------------------------------------------
	bool directory_iterator::has_next () const {
		return _ops->has_next (_ops->opaque);
	}

	// ----------------------------------------------------------------------
	const inode* directory_iterator::owner () const {
		return _owner;
	}

	// ----------------------------------------------------------------------
	std::string directory_iterator::next () {
		std::string s;
		const auto max_len = _owner->owner ()->max_name_length ();
		s.resize (max_len);
		size_t n = _ops->next (_ops->opaque, const_cast<char*>(s.c_str ()), max_len);
		if (n < max_len) {
			s.resize (n);
		} else {
			s[max_len-1] = 0;
		}
		return s;
	}

	// ----------------------------------------------------------------------
	directory_iterator::directory_iterator (vfs_directory_iterator* ops, const inode* owner)
		: _ops (ops), _owner (owner) {
	}

	// ========================================================================
	std::unique_ptr<inode> inode::lookup (const std::string& name) const {
		vfs_inode_ops* child = _ops->lookup (_ops->opaque, const_cast<char*>(name.c_str ()));
		if (!child) {
			return nullptr;
		}
		return std::unique_ptr<inode> (new inode (child, _owner));
	}

	// ----------------------------------------------------------------------
	void inode::stat (stats& st) const {
		if (!_ops->stat (_ops->opaque, &st)) {
			THROW_EXCEPTION_EX(vfs::exception, "failed to load stat");
		}
	}

	// ----------------------------------------------------------------------
	std::unique_ptr<directory_iterator> inode::get_directory_iterator () const {
		vfs_directory_iterator* itr = _ops->get_directory_iterator (_ops->opaque);
		if (!itr) {
			return nullptr;
		}
		return std::unique_ptr<directory_iterator> (new directory_iterator (itr, this));
	}

	// ----------------------------------------------------------------------
	std::unique_ptr<file_ops> inode::get_file_ops (open_mode_type mode_type) const {
		vfs_file_ops* ops = _ops->open_file (_ops->opaque, mode_type);
		if (!ops) {
			return nullptr;
		}
		return std::unique_ptr<file_ops> (new file_ops (ops, const_cast<inode*>(this)));
	}

	// ----------------------------------------------------------------------
	bool inode::mkdir (const std::string& name) {
		auto res = _ops->mkdir (_ops->opaque, const_cast<char*>(name.c_str ())) == 1;
		if (res) {
			_make_dirty ();
		}
		return res;
	}

	// ----------------------------------------------------------------------
	bool inode::mkfile (const std::string& name) {
		auto res = _ops->mkfile (_ops->opaque, const_cast<char*>(name.c_str ())) == 1;
		if (res) {
			_make_dirty ();
		}
		return res;
	}

	// ----------------------------------------------------------------------
	inode::inode (vfs_inode_ops* ops, file_system* owner)
		: _ops (ops),
		  _owner (owner),
		  _dirty (false) {

	}

	// ----------------------------------------------------------------------
	inode::~inode () {
		if (_ops) {
			_ops->destructor (_ops);
		}
	}

	// ----------------------------------------------------------------------
	const file_system* inode::owner () const {
		return _owner;
	}

	// ----------------------------------------------------------------------
	bool inode::dirty () const noexcept {
		return _dirty;
	}

	// ----------------------------------------------------------------------
	int inode::sync () {
		if (!is_readonly ()) {
			return _owner->sync (this);
		}
		return 1;
	}

	// ----------------------------------------------------------------------
	int inode::unlink () {
		auto res = _ops->unlink (_ops->opaque);
		if (res) {
			_make_dirty ();
		}
		return res;
	}

	// ----------------------------------------------------------------------
	void inode::_make_dirty () {
		if (!is_readonly ()) {
			_dirty = true;
		}
	}

	// ----------------------------------------------------------------------
	bool inode::is_readonly () const {
		return _owner->is_readonly ();
	}

	// ========================================================================
	bool file_ops::seek (uint64_t pos, enum whence_type whence) {
		return _ops->seek (_ops->opaque, pos, whence) == 1;
	}

	// ------------------------------------------------------------------------
	uint64_t file_ops::tell () const {
		return _ops->tell (_ops->opaque);
	}

	// ------------------------------------------------------------------------
	ssize_t file_ops::read (void* buff, size_t len) {
		return _ops->read (_ops->opaque, buff, len);
	}

	// ------------------------------------------------------------------------
	ssize_t file_ops::write (const void* buff, size_t len) {
		const ssize_t rc = _ops->write (_ops->opaque, const_cast<void*>(buff), len);
		if (rc > 0) {
			_owner->_make_dirty ();
		}
		return rc;
	}

	// ------------------------------------------------------------------------
	bool file_ops::truncate () {
		if (_ops->truncate (_ops->opaque)) {
			_owner->_make_dirty ();
			return true;
		}
		return false;
	}

	// ------------------------------------------------------------------------
	file_ops::~file_ops () {
		_ops->destructor (_ops);
	}

	// ------------------------------------------------------------------------
	const inode* file_ops::owner () const {
		return _owner;
	}

	// ------------------------------------------------------------------------
	bool file_ops::is_readonly () const {
		return _owner->is_readonly ();
	}

	// ------------------------------------------------------------------------
	file_ops::file_ops (vfs_file_ops* ops, inode* owner)
		: _ops (ops),
		  _owner (owner) {

	}
} // ns vfs
