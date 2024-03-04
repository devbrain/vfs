//
// Created by igor on 2/4/24.
//

#include "io_iface.hh"
#include "detail/path.hh"
#include "detail/file_system.hh"
#include "detail/dentry.hh"
#include "detail/stats_converter.hh"
#include "detail/fstab.hh"
#include "vfs/exception.hh"

namespace vfs {
	struct file {
		file (std::unique_ptr<core::file_ops>&& fops, std::string  pth, bool ro, bool app, bool seq)
			: file_ops (std::move (fops)),
			  file_path (std::move(pth)),
			  read_only (ro),
			  append (app),
			  is_sequential (seq) {

		}

		std::unique_ptr<core::file_ops> file_ops;
		std::string file_path;
		bool read_only;
		bool append;
		bool is_sequential;
	};

	namespace core {
		std::optional<vfs::stats> get_stats (const std::string& pth, const dentry_tree* tree) {
			path some_path (pth);
			some_path.make_directory ();

			auto [dent, ino, depth, p] = tree->resolve (some_path);

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
		vfs::directory open_directory (const std::string& pth, dentry_tree* tree) {
			path some_path (pth);
			some_path.make_directory ();

			auto [dent, ino, depth, p] = tree->resolve (some_path);
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
		void create_directory (const std::string& pth, const dentry_tree* tree) {
			path some_path (pth);
			some_path.make_directory ();

			auto [dent, ino, depth, p] = tree->resolve (some_path);
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
		void unlink (const std::string& pth, const dentry_tree* tree, fstab* fs_table) {
			path some_path (pth);
			some_path.make_directory ();

			auto [dent, ino, depth, p] = tree->resolve (some_path);
			if (depth != p.depth ()) {
				THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
			}
			if (dentry_tree::has_children (dent)) {
				THROW_EXCEPTION_EX(vfs::exception, "path ", pth, " is non empty directory");

			}
			if (ino->is_readonly ()) {
				THROW_EXCEPTION_EX(vfs::exception, "failed to unlink ", pth, " Read only file system");
			}
			if (!ino->unlink ()) {
				THROW_EXCEPTION_EX(vfs::exception, "failed to unlink ", pth);
			}
			if (dentry_tree::unlink (dent)) {
				p.make_directory ();
				fs_table->unmount (p);
			}
		}
		// ------------------------------------------------------------------------------------------------
		file* open (const std::string& pth, unsigned openmode, const dentry_tree* tree) {
			path some_path (pth);
			some_path.make_directory ();
			auto [dent, ino, depth, p] = tree->resolve (some_path);
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
						return new file (std::move (fops), pth, read_only, has_append, st.is_sequential);
					} else {
						if (openmode & CREATE) {
							return new file (std::move (fops), pth, read_only, has_append, st.is_sequential);
						} else {
							THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, "for TRUNCATE");
						}
					}
				} else {
					return new file (std::move (fops), pth, read_only, has_append, st.is_sequential);
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
					auto [dent_new, ino_new, depth_new, new_path] = tree->resolve (p);
					if (depth_new < new_path.depth ()) {
						THROW_EXCEPTION_EX(vfs::exception, "can not resolve new file ", pth);
					}
					fops = ino_new->get_file_ops ((openmode & WRITE_ONLY) ? eVFS_OPEN_MODE_WRITE : eVFS_OPEN_MODE_READ);
					if (!fops) {
						THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " for writing");
					}
					return new file (std::move (fops), pth, read_only, has_append, ino_new->is_sequential());
				} else {
					THROW_EXCEPTION_EX(vfs::exception, "path does not exists ", pth);
				}
			}
			THROW_EXCEPTION_EX(exception, "Should not be here ", pth);
		}
		// ------------------------------------------------------------------------------------------
		file* open (const std::string& pth, creation_disposition cd, bool readonly, const dentry_tree* tree) {
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
			auto [dent, ino, depth, p] = tree->resolve (some_path);
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
						return new file (std::move (fops), pth, readonly, false, ino->is_sequential());
					case creation_disposition::eCREATE_NEW:
					case creation_disposition::eOPEN_EXISTING:
						fops = ino->get_file_ops (readonly ? eVFS_OPEN_MODE_READ : eVFS_OPEN_MODE_WRITE);
						if (!fops) {
							THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth);
						}
						return new file (std::move (fops), pth, readonly, false, ino->is_sequential());
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
					auto [dent_new, ino_new, depth_new, new_path] = tree->resolve (p);
					if (depth_new < new_path.depth ()) {
						THROW_EXCEPTION_EX(vfs::exception, "can not resolve new file ", pth);
					}
					fops = ino_new->get_file_ops (eVFS_OPEN_MODE_WRITE);
					if (!fops) {
						THROW_EXCEPTION_EX(vfs::exception, "unable to open file ", pth, " for writing");
					}
					return new file (std::move (fops), pth, readonly, false, ino_new->is_sequential());
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
			if (f->is_sequential) {
				THROW_EXCEPTION_EX(vfs::exception, "sequential access only for ", f->file_path);
			}
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
	}
}