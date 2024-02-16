//
// Created by igor on 2/16/24.
//

#include "mounts_model.hh"
#include <vfs/system.hh>

MountsModel::MountsModel (QObject* parent)
	: QAbstractListModel (parent) {
	populate();
}

void MountsModel::populate () {
	beginResetModel ();
	m_mounts.clear();
	auto mounts = vfs::get_mounts();
	for (auto i = mounts.begin(); i != mounts.end(); i++) {
		auto d = *i;
		m_mounts.emplace_back (d.path(), d.type(), d.args(), d.is_readonly());
	}
	endResetModel();
}

int MountsModel::rowCount (const QModelIndex& parent) const {
	return static_cast<int>(m_mounts.size());
}

int MountsModel::columnCount (const QModelIndex& parent) const {
	return 4;
}

QVariant MountsModel::headerData (int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
			case 0:
				return "Type";
			case 1:
				return "Path";
			case 2:
				return "Args";
			case 3:
				return "ReadOnly";
			default:
				return {};
		}
	}
	return QAbstractItemModel::headerData (section, orientation, role);
}

QVariant MountsModel::data (const QModelIndex& index, int role) const {
	if (!index.isValid ()) {
		return {};
	}
	if (role == Qt::DisplayRole) {
		const auto& d = m_mounts[index.row()];
		switch (index.column()) {
			case 0:
				return d.type;
			case 1:
				return d.path;
			case 2:
				return d.args;
			case 3:
				return d.readonly ? "Read Only" : "Writable";
		}
	}
	return {};
}

MountsModel::MountData::MountData (const std::string& path, const std::string& type, const std::string& args, bool readonly)
	: path (path.c_str()), type (type.c_str()), args (args.c_str()), readonly (readonly) {}
