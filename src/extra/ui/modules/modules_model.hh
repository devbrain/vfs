//
// Created by igor on 2/16/24.
//

#ifndef VFS_SRC_EXTRA_UI_MODULES_MODULES_MODEL_HH_
#define VFS_SRC_EXTRA_UI_MODULES_MODULES_MODEL_HH_

#include <vector>
#include <list>
#include <QAbstractListModel>

struct ModulesLoadingReport {
	ModulesLoadingReport (const QString& path, bool loaded);
	QString path;
	bool loaded;
};

class ModulesModel : public QAbstractListModel {
	Q_OBJECT
 public:
	explicit ModulesModel(QObject *parent = nullptr);
	std::list<ModulesLoadingReport> load(const QString& path, bool with_report);
 public slots:
	void populate();
 private:
	[[nodiscard]] int rowCount(const QModelIndex& parent) const override;
	[[nodiscard]] int columnCount(const QModelIndex& parent) const override;
	[[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
	[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  private:
	struct ModuleData {
		QString type;
		int     refcount;
		QString path;
	};
	std::vector<ModuleData> m_data;
};

#endif //VFS_SRC_EXTRA_UI_MODULES_MODULES_MODEL_HH_
