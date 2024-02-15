//
// Created by igor on 2/11/24.
//

#ifndef VFS_SRC_EXTRA_UI_NAVIGATION_PANEL_HH_
#define VFS_SRC_EXTRA_UI_NAVIGATION_PANEL_HH_

#include <QFrame>
#include <QLayout>

class NavigationModel;
class NavigationView;

class NavigationPanel : public QFrame {
	Q_OBJECT
 public:
	explicit NavigationPanel(QWidget* parent);

	QString getCurrentPath() const;
 public slots:
	void setActive(bool flag);
	void toggleActive();
 signals:
	void tabPressed();
	void currentPathChanged(const QString& newPath);
 private:
	QVBoxLayout* m_layout;
	NavigationModel* m_model;
	NavigationView* m_view;
 private:
	bool m_is_active;
};

#endif //VFS_SRC_EXTRA_UI_NAVIGATION_PANEL_HH_
