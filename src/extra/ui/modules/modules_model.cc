//
// Created by igor on 2/16/24.
//

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
	endResetModel();
}






