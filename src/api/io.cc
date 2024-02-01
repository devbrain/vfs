//
// Created by igor on 2/1/24.
//
#include <vfs/io.hh>
#include <vfs/api/exception.hh>
#include "detail/dentry.hh"
#include "detail/stats_converter.hh"

namespace vfs {
	extern void fstab_unmount(const path& p);

	std::optional<stats> get_stats (const std::string& pth) {
		path some_path (pth);
		some_path.make_directory ();

		auto [dent, ino, depth, p] = core::dentry_resolve (some_path);

		if (depth != p.depth ()) {
			return std::nullopt;
		}

		core::stats st;
		try {
			ino->stat (st);
		} catch (...) {
			return std::nullopt;
		}

		auto res = convert (st);

		return std::make_optional (res);
	}

	// ---------------------------------------------------------------------------------
	directory open_directory (const std::string& pth) {
		path some_path (pth);
		some_path.make_directory ();

		auto [dent, ino, depth, p] = core::dentry_resolve (some_path);
		if (depth != p.depth ()) {
			THROW_EXCEPTION_EX(vfs::exception, "path not found ", pth);
		}
		core::stats st;
		ino->stat (st);
		if (st.type != VFS_INODE_DIRECTORY) {
			THROW_EXCEPTION_EX(vfs::exception, "path ", pth, " is not directory");
		}
		return directory (ino);
	}

	// ---------------------------------------------------------------------------------
	void create_directory (const std::string& pth) {
		path some_path (pth);
		some_path.make_directory ();

		auto [dent, ino, depth, p] = core::dentry_resolve (some_path);
		if (depth == p.depth ()) {
			THROW_EXCEPTION_EX(vfs::exception, "path ", pth, " already exists");
		}
		if (depth != p.depth () - 1) {
			THROW_EXCEPTION_EX(vfs::exception, "path ", pth, "has to many non existing components");
		}
		if (ino->is_readonly ()) {
			THROW_EXCEPTION_EX(vfs::exception, "failed to create directory ", pth, " Read only file system");
		}
		p.make_file ();
		if (!ino->mkdir (p.get_file_name ())) {
			THROW_EXCEPTION_EX(vfs::exception, "failed to create directory ", pth);
		}
	}

	// --------------------------------------------------------------------------------
	void unlink (const std::string& pth) {
		path some_path (pth);
		some_path.make_directory ();

		auto [dent, ino, depth, p] = core::dentry_resolve (some_path);
		if (depth != p.depth ()) {
			THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
		}
		if (dentry_has_children (dent)) {
			THROW_EXCEPTION_EX(vfs::exception, "path ", pth, " is non empty directory");

		}
		if (ino->is_readonly ()) {
			THROW_EXCEPTION_EX(vfs::exception, "failed to unlink ", pth, " Read only file system");
		}
		if (!ino->unlink ()) {
			THROW_EXCEPTION_EX(vfs::exception, "failed to unlink ", pth);
		}
		if (dentry_unlink (dent)) {
			p.make_directory ();
			fstab_unmount (p);
		}
	}

	// ------------------------------------------------------------------------------------------
	struct file {
		file (std::unique_ptr<core::file_ops>&& fops, std::string  pth, bool ro, bool app)
			: file_ops (std::move (fops)),
			  file_path (std::move(pth)),
			  read_only (ro),
			  append (app) {

		}

		std::unique_ptr<core::file_ops> file_ops;
		std::string file_path;
		bool read_only;
		bool append;
	};

	// ------------------------------------------------------------------------------------------
	file* open (const std::string& pth, unsigned openmode) {
		path some_path (pth);
		some_path.make_directory ();
		auto [dent, ino, depth, p] = core::dentry_resolve (some_path);
		std::unique_ptr<core::file_ops> fops;
		bool read_only = (openmode & READ_ONLY) && ((openmode & WRITE_ONLY) == 0);
		bool has_append = (openmode & APPEND);

		if (depth == p.depth ()) {
			// path found
			core::stats st;
			ino->stat (st);
			if (st.type == VFS_INODE_DIRECTORY) {
				THROW_EXCEPTION_EX(vfs::exception, "path ", pth, " is directory");
			}
			fops = ino->get_file_ops (read_only ? eVFS_OPEN_MODE_READ : eVFS_OPEN_MODE_WRITE);
			if (!fops) {
				THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth);
			}

			if (openmode & TRUNCATE) {
				if ((openmode & WRITE_ONLY) || (openmode & READ_ONLY)) {
					if (!fops->truncate ()) {
						THROW_EXCEPTION_EX(vfs::exception, "failed to truncate file ", pth);
					}
					return new file (std::move (fops), pth, read_only, has_append);
				} else {
					if (openmode & CREATE) {
						return new file (std::move (fops), pth, read_only, has_append);
					} else {
						THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, "for TRUNCATE");
					}
				}
			} else {
				return new file (std::move (fops), pth, read_only, has_append);
			}
		} else {
			if (depth == p.depth () - 1) {
				if (read_only) {
					THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " for reading, since it does not exists");
				}
				p.make_file ();
				if (!ino->mkfile (p.get_file_name ())) {
					THROW_EXCEPTION_EX(vfs::exception, "can not create new file ", pth);
				}
				p.make_directory ();
				auto [dent_new, ino_new, depth_new, new_path] = core::dentry_resolve (p);
				if (depth_new < new_path.depth ()) {
					THROW_EXCEPTION_EX(vfs::exception, "can not resolve new file ", pth);
				}
				fops = ino_new->get_file_ops ((openmode & WRITE_ONLY) ? eVFS_OPEN_MODE_WRITE : eVFS_OPEN_MODE_READ);
				if (!fops) {
					THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " for writing");
				}
				return new file (std::move (fops), pth, read_only, has_append);
			} else {
				THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
			}
		}
		THROW_EXCEPTION_EX(exception, "Should not be here ", pth);
	}
	// ------------------------------------------------------------------------------------------
	file* open (const std::string& pth, creation_disposition cd, bool readonly) {
		path some_path (pth);
		some_path.make_directory ();
		if (readonly) {
			if (cd == creation_disposition::eCREATE_ALWAYS) {
				THROW_EXCEPTION_EX(vfs::exception, "Mismatched arguments CREATE_ALWAYS and readonly");
			}
			if (cd == creation_disposition::eTRUNCATE_EXISTING) {
				THROW_EXCEPTION_EX(vfs::exception, "Mismatched arguments TRUNCATE_EXISTING and readonly");
			}

		}
		// TODO check read only fs
		auto [dent, ino, depth, p] = core::dentry_resolve (some_path);
		std::unique_ptr<core::file_ops> fops;

		if (depth == p.depth ()) {
			switch (cd) {
				case creation_disposition::eCREATE_ALWAYS:
				case creation_disposition::eTRUNCATE_EXISTING:
					if (ino->is_readonly ()) {
						THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " Read only file system");
					}
					fops = ino->get_file_ops (eVFS_OPEN_MODE_WRITE);
					if (!fops) {
						THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth);
					}
					if (!fops->truncate ()) {
						THROW_EXCEPTION_EX(vfs::exception, "failed to truncate file ", pth);
					}
					return new file (std::move (fops), pth, readonly, false);
				case creation_disposition::eCREATE_NEW:
				case creation_disposition::eOPEN_EXISTING:
					fops = ino->get_file_ops (readonly ? eVFS_OPEN_MODE_READ : eVFS_OPEN_MODE_WRITE);
					if (!fops) {
						THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth);
					}
					return new file (std::move (fops), pth, readonly, false);
			}
		} else {
			if (depth == p.depth () - 1) {
				p.make_file ();

				if (readonly) {
					THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " for reading, since it does not exists");
				}
				if (cd == creation_disposition::eOPEN_EXISTING || cd == creation_disposition::eTRUNCATE_EXISTING) {
					THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
				}
				if (!ino->mkfile (p.get_file_name ())) {
					THROW_EXCEPTION_EX(vfs::exception, "can not create new file ", pth);
				}
				p.make_directory ();
				auto [dent_new, ino_new, depth_new, new_path] = core::dentry_resolve (p);
				if (depth_new < new_path.depth ()) {
					THROW_EXCEPTION_EX(vfs::exception, "can not resolve new file ", pth);
				}
				fops = ino_new->get_file_ops (eVFS_OPEN_MODE_WRITE);
				if (!fops) {
					THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " for writing");
				}
				return new file (std::move (fops), pth, readonly, false);
			} else {
				THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
			}
		}
		THROW_EXCEPTION_EX(exception, "Should not be here ", pth);
	}

	// ---------------------------------------------------------------------------------------------------------------
	void close (file* f) {
		if (f && f->file_ops) {
			delete f;
		}
	}

	// ---------------------------------------------------------------------------------------------------------------
	size_t read (file* f, void* buff, size_t len) {
		if (f && f->file_ops) {
			auto rc = f->file_ops->read (buff, len);
			if (rc < 0) {
				THROW_EXCEPTION_EX(vfs::exception, "read failed from ", f->file_path);
			}
			return static_cast<size_t>(rc);
		}
		THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
	}

	// ---------------------------------------------------------------------------------------------------------------
	size_t write (file* f, const void* buff, size_t len) {
		if (f && f->file_ops) {
			if (f->read_only) {
				THROW_EXCEPTION_EX(vfs::exception, "the file ", f->file_path, " is opened in read only mode");
			}
			if (f->file_ops->is_readonly ()) {
				THROW_EXCEPTION_EX(vfs::exception, "the file ", f->file_path, " is opened in read only file system");
			}
			if (f->append) {
				if (!f->file_ops->seek (0, eVFS_SEEK_END)) {
					THROW_EXCEPTION_EX(vfs::exception, "append failed ", f->file_path);
				}
			}
			auto rc = f->file_ops->write (buff, len);
			if (rc < 0) {
				THROW_EXCEPTION_EX(vfs::exception, "write failed to ", f->file_path);
			}

			return static_cast<size_t>(rc);
		}
		THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
	}

	// ---------------------------------------------------------------------------------------------------------------
	bool truncate (file* f) {
		if (f && f->file_ops) {
			return f->file_ops->truncate ();
		}
		THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
	}

	// ---------------------------------------------------------------------------------------------------------------
	void seek (file* f, uint64_t pos, seek_type whence) {
		if (f && f->file_ops) {
			whence_type wt = eVFS_SEEK_CUR;
			switch (whence) {
				case seek_type::eCUR:wt = eVFS_SEEK_CUR;
					break;
				case seek_type::eEND:wt = eVFS_SEEK_END;
					break;
				case seek_type::eSET:wt = eVFS_SEEK_SET;
					break;
			}
			auto rc = f->file_ops->seek (pos, wt);
			if (!rc) {
				THROW_EXCEPTION_EX(vfs::exception, "seek to ", pos, " failed in ", f->file_path);
			}

		} else {
			THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
		}
	}

	// ---------------------------------------------------------------------------------
	uint64_t tell (file* f) {
		if (f && f->file_ops) {
			auto rc = f->file_ops->tell ();
			if (rc == static_cast<uint64_t>(-1)) {
				THROW_EXCEPTION_EX(vfs::exception, "tell failed for ", f->file_path);
			}
			return rc;
		}
		THROW_EXCEPTION_EX(vfs::exception, "Invalid file handle");
	}
	// -----------------------------------------------------------------------------------

}