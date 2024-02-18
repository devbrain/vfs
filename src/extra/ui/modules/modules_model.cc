//
// Created by igor on 2/16/24.
//

#include <algorithm>
#include <filesystem>
#include "modules_model.hh"
#include <vfs/system.hh>

ModulesModel::ModulesModel (QObject* parent)
	: QAbstractListModel (parent) {
	populate();
}

int ModulesModel::rowCount (const QModelIndex& parent) const {
	return static_cast<int>(m_data.size());
}

int ModulesModel::columnCount (const QModelIndex& parent) const {
	return 3;
}

QVariant ModulesModel::data (const QModelIndex& index, int role) const {
	if (!index.isValid ()) {
		return {};
	}
	if (role == Qt::DisplayRole) {
		const auto& d = m_data[index.row()];
		switch (index.column()) {
			case 0:
				return d.type;
			case 1:
				return d.path.isEmpty() ? "<BUILT-IN>" : d.path;
			case 2:
				return d.refcount;
		}
	}
	return {};
}

QVariant ModulesModel::headerData (int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
			case 0:
				return "Type";
			case 1:
				return "Path";
			case 2:
				return "RefCount";
			default:
				return {};
		}
	}
	return QAbstractItemModel::headerData (section, orientation, role);
}

void ModulesModel::populate () {
	beginResetModel ();
	m_data.clear();
	auto modules = vfs::get_modules();
	for (auto i = modules.begin(); i != modules.end(); i++) {
		auto d = *i;
		ModuleData md;
		md.path = d.path().c_str();
		md.type = d.type().c_str();
		md.refcount = d.refcount();
		m_data.push_back (md);
	}
	std::sort(m_data.begin(), m_data.end(), [](const auto& a, const auto& b) {
		return a.type < b.type;
	});
	endResetModel();
}

std::list<ModulesLoadingReport> ModulesModel::load(const QString& path, bool with_report) {
	std::filesystem::path p(path.toStdString());
	vfs::modules_loading_report report;
	vfs::load_module (p, &report);
	populate();
	std::list<ModulesLoadingReport> out;
	if (with_report) {
		for (const auto& entry : report) {
			out.emplace_back (entry.path.c_str(), entry.is_loaded);
		}
	}
	return out;
}

ModulesLoadingReport::ModulesLoadingReport (const QString& path, bool loaded)
	: path (path), loaded (loaded) {}
