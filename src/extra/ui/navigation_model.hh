//
// Created by igor on 2/7/24.
//

#ifndef VFS_SRC_EXTRA_UI_NAVIGATION_MODEL_HH_
#define VFS_SRC_EXTRA_UI_NAVIGATION_MODEL_HH_

#include <string>
#include <vector>
#include <filesystem>
#include <QAbstractListModel>


class NavigationModel : public QAbstractListModel {
	Q_OBJECT
 public:
	explicit NavigationModel (const QString& initalPath, QObject *parent = nullptr);
 public slots:
	void moveDown();
 	void moveUp();
 private:
	[[nodiscard]] int rowCount(const QModelIndex& parent) const override;
	[[nodiscard]] int columnCount(const QModelIndex& parent) const override;
	[[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
	[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
 private:
	void populate(const std::filesystem::path& path);

	struct Entry {
		Entry (const std::string& name, bool is_dir, uint64_t size);
		QString name;
		bool    isDir;
		uint64_t size;
	};

	std::filesystem::path m_currentPath;
	std::vector<Entry> m_entries;
	int m_currentIndex;

};

#endif //VFS_SRC_EXTRA_UI_NAVIGATION_MODEL_HH_
