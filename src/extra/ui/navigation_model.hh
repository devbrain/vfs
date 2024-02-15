//
// Created by igor on 2/7/24.
//

#ifndef VFS_SRC_EXTRA_UI_NAVIGATION_MODEL_HH_
#define VFS_SRC_EXTRA_UI_NAVIGATION_MODEL_HH_

#include <string>
#include <vector>
#include <filesystem>
#include <QAbstractListModel>

#include "navigation_cursor.hh"


class NavigationModel : public QAbstractListModel {
	Q_OBJECT
 public:
	explicit NavigationModel (const QString& initalPath, QObject *parent = nullptr);

	QString getCurrentPath() const;
 public slots:
	void onNavigateDown(JumpMode jumpMode);
	void onNavigateUp(JumpMode jumpMode);
	void onDrillDown ();
	void onDrillDownByIndex(const QModelIndex index);
	void setActive(bool f);
 signals:
	void modelPopulated();
	void cursorMoved(const QModelIndex& newPos);
	void currentPathChanged(const QString& newPath);
 private:
	[[nodiscard]] int rowCount(const QModelIndex& parent) const override;
	[[nodiscard]] int columnCount(const QModelIndex& parent) const override;
	[[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
	[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
 private:
	void populate(const std::filesystem::path& path);
	void moveCursor(int newPos);
	void drillDownByRow(int row);
 private:
	struct Entry {
		Entry (const std::string& name, bool is_dir, uint64_t size);
		QString name;
		bool    isDir;
		uint64_t size;
	};
	std::filesystem::path m_initial_path;
	std::filesystem::path m_current_path;
	std::vector<Entry> m_entries;
	int m_cursor;
	bool m_is_active;
};

#endif //VFS_SRC_EXTRA_UI_NAVIGATION_MODEL_HH_
