//
// Created by igor on 2/16/24.
//

#ifndef VFS_SRC_EXTRA_UI_MOUNTS_MOUNTS_MODEL_HH_
#define VFS_SRC_EXTRA_UI_MOUNTS_MOUNTS_MODEL_HH_

#include <vector>
#include <QAbstractListModel>

class MountsModel : public QAbstractListModel{
 public:
	explicit MountsModel(QObject *parent = nullptr);
 private:
	[[nodiscard]] int rowCount(const QModelIndex& parent) const override;
	[[nodiscard]] int columnCount(const QModelIndex& parent) const override;
	[[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
	[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
 private:
	void populate();
 private:
	struct MountData {
		MountData (const std::string& path, const std::string& type, const std::string& args, bool readonly);
		QString path;
		QString type;
		QString args;
		bool    readonly;
	};
	std::vector<MountData> m_mounts;
};

#endif //VFS_SRC_EXTRA_UI_MOUNTS_MOUNTS_MODEL_HH_
