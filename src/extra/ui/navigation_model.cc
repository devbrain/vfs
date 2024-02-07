//
// Created by igor on 2/7/24.
//
#include <algorithm>
#include <QColor>

#include <bsw/exception.hh>
#include <vfs/io.hh>

#include "navigation_model.hh"

NavigationModel::NavigationModel (const QString& initalPath, QObject *parent)
:   QAbstractListModel (parent),
    m_currentPath(initalPath.toStdString()),
	m_currentIndex (0) {
	populate (m_currentPath);
}

int NavigationModel::rowCount (const QModelIndex& parent) const {
	return static_cast<int>(m_entries.size());
}

int NavigationModel::columnCount (const QModelIndex& parent) const {
	return 2;
}

QVariant NavigationModel::data (const QModelIndex& index, int role) const {
	if (!index.isValid()) {
		return {};
	}
   if ( role == Qt::DisplayRole)
   {
	   const auto& e = m_entries[index.row ()];
       if (index.column() == 0) {
		   return e.name;
	   }
       if (index.column() == 1) {
			if (e.isDir) {
				return "<DIR>";
			} else {
				return static_cast<qulonglong>(e.size);
			}
	   }
   }
   if (role == Qt::BackgroundRole) {
	   if (index.row() == m_currentIndex) {
		   return QColor("yellow");
	   }
   }
   return {};
}

void NavigationModel::populate (const std::filesystem::path& path) {
	auto u8_path = path.u8string();
	auto stats = vfs::get_stats (u8_path);
	if (stats) {
		if (stats->type == vfs::stats::type_t::eDIRECTORY) {
			beginResetModel();
			m_entries.clear();
			m_currentIndex = 0;
			auto dir = vfs::open_directory (u8_path);
			vfs::directory_iterator end;
			for (vfs::directory_iterator dir_itr(dir); dir_itr != end; dir_itr++) {
				const auto& [name, st] = *dir_itr;
				m_entries.emplace_back (name, st.type == vfs::stats::type_t::eDIRECTORY, st.size);
				std::sort(m_entries.begin(), m_entries.end(), [](const auto& a, const auto& b) {
					if ((a.isDir && b.isDir) || (!a.isDir && !b.isDir)) {
						return a.name < b.name;
					}
					return a.isDir;
				});
			}
			endResetModel();
		} else {
			RAISE_EX(u8_path, "should be directory");
		}
	} else {
		RAISE_EX("Path", u8_path, "is not found");
	}
}

QVariant NavigationModel::headerData (int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		if (section == 0) {
			return "Name";
		}
		if (section == 1) {
			return "Size";
		}
	}
	return {};
}

void NavigationModel::moveDown () {
	if (m_currentIndex < m_entries.size() - 1) {
		m_currentIndex++;
		emit dataChanged(index(m_currentIndex-1,0), index(m_currentIndex-1, 2));
		emit dataChanged(index(m_currentIndex,0), index(m_currentIndex, 2));
	}
}

void NavigationModel::moveUp () {
	if (m_currentIndex > 0) {
		m_currentIndex--;
		emit dataChanged(index(m_currentIndex+1,0), index(m_currentIndex+1, 2));
		emit dataChanged(index(m_currentIndex,0), index(m_currentIndex, 2));
	}
}



NavigationModel::Entry::Entry (const std::string& name, bool is_dir, uint64_t size)
	: name (name.c_str()), isDir (is_dir), size (size) {}
