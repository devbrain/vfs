//
// Created by igor on 2/7/24.
//
#include <algorithm>
#include <QColor>

#include <bsw/exception.hh>
#include <vfs/io.hh>

#include "navigation_model.hh"

NavigationModel::NavigationModel (const QString& initalPath, QObject* parent)
	: QAbstractListModel (parent),
	  m_initial_path(initalPath.toStdString ()),
	  m_current_path (m_initial_path),
	  m_cursor (0),
      m_is_active (true) {
	populate (m_current_path);
}

int NavigationModel::rowCount (const QModelIndex& parent) const {
	return static_cast<int>(m_entries.size ());
}

int NavigationModel::columnCount (const QModelIndex& parent) const {
	return 2;
}

QVariant NavigationModel::data (const QModelIndex& index, int role) const {
	if (!index.isValid ()) {
		return {};
	}
	if (role == Qt::DisplayRole) {
		const auto& e = m_entries[index.row ()];
		if (index.column () == 0) {
			return e.name;
		}
		if (index.column () == 1) {
			if (e.isDir) {
				return "<DIR>";
			} else {
				return static_cast<qulonglong>(e.size);
			}
		}
	}
	if (m_is_active && role == CURSOR_ROLE) {
		return index.row() == m_cursor;
	}
	if (m_is_active && role == Qt::BackgroundRole) {
		if (index.row() == m_cursor) {
			return QColor::fromRgb (100, 0, 0);
		}
	}
	return {};
}

void NavigationModel::onDrillDown () {
	drillDownByRow (m_cursor);
}

void NavigationModel::onDrillDownByIndex(const QModelIndex index) {
	if (index.isValid()) {
		drillDownByRow (index.row ());
	}
}

void NavigationModel::drillDownByRow(int row) {
	if (row >= 0 && row < m_entries.size()  && m_entries[row].isDir) {
		auto name = m_entries[row].name.toStdString();
		if (name == "..") {
			populate (m_current_path.parent_path());
		} else {
			populate (m_current_path / name);
		}
	}
}

void NavigationModel::populate (const std::filesystem::path& path) {
	auto u8_path = path.u8string ();
	auto stats = vfs::get_stats (u8_path);
	if (stats) {
		if (stats->type == vfs::stats::type_t::eDIRECTORY) {
			m_current_path = path;
			emit currentPathChanged ({m_current_path.c_str()});
			beginResetModel ();
			m_entries.clear ();
			m_cursor = 0;

			if (m_current_path != m_initial_path) {
				m_entries.emplace_back ("..", true, 0);
			}

			auto dir = vfs::open_directory (u8_path);
			vfs::directory_iterator end;
			for (vfs::directory_iterator dir_itr (dir); dir_itr != end; dir_itr++) {
				const auto& [name, st] = *dir_itr;
				if (st.type != vfs::stats::type_t::eNAME_TRUNCATED) {
					m_entries.emplace_back (name, st.type == vfs::stats::type_t::eDIRECTORY, st.size);
				}
			}
			std::sort (m_entries.begin (), m_entries.end (), [] (const auto& a, const auto& b) {
			  if ((a.isDir && b.isDir) || (!a.isDir && !b.isDir)) {
				  return a.name < b.name;
			  }
			  return a.isDir;
			});
			endResetModel ();
			emit modelPopulated ();
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

void NavigationModel::onNavigateDown(JumpMode jumpMode) {
	const auto last_row = rowCount (QModelIndex()) - 1;
	int new_pos = 0;
	if (jumpMode == Edge) {
		new_pos = last_row;
	} else {
		new_pos = jumpMode == Jump ? std::min (m_cursor + 10, last_row) :
				  std::min (m_cursor + 1, last_row);
	}
	moveCursor (new_pos);
}

void NavigationModel::onNavigateUp(JumpMode jumpMode) {
	int new_pos = 0;
	if (jumpMode == Edge) {
		new_pos = 0;
	} else {
		new_pos = jumpMode == Jump ? std::max (m_cursor - 10, 0) : std::max (m_cursor - 1, 0);
	}
	moveCursor (new_pos);
}

void NavigationModel::moveCursor(int newPos) {
	if (m_cursor != newPos) {
		auto startIndex = index (m_cursor, 0);
		emit cursorMoved (startIndex);
		m_cursor = newPos;
		emit cursorMoved (index (m_cursor, 0));
		auto endIndex = index (newPos, columnCount (QModelIndex{}) - 1);
		emit dataChanged (startIndex, endIndex);
	}
}

void NavigationModel::setActive(bool f) {
	m_is_active = f;
	auto startIndex = index (m_cursor, 0);
	auto endIndex = index (m_cursor, columnCount (QModelIndex{}) - 1);
	emit dataChanged (startIndex, endIndex);
}

QString NavigationModel::getCurrentPath () const {
	return {m_current_path.c_str()};
}

NavigationModel::Entry::Entry (const std::string& name, bool is_dir, uint64_t size)
	: name (name.c_str ()), isDir (is_dir), size (size) {}
