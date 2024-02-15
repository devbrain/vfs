//
// Created by igor on 2/7/24.
//

#ifndef VFS_SRC_EXTRA_UI_NAVIGATION_VIEW_HH_
#define VFS_SRC_EXTRA_UI_NAVIGATION_VIEW_HH_

#include <map>
#include <functional>
#include <QTreeView>
#include "navigation_cursor.hh"

class NavigationView : public QTreeView {
	Q_OBJECT
 public:
	explicit NavigationView(QWidget* parent = nullptr);
 public slots:
	void onModelPopulated();
 	void onCursorMoved(const QModelIndex& newPos);
 signals:
	void navigateDown(JumpMode jumpMode);
	void navigateUp(JumpMode jumpMode);
	void drillDown();
	void drillDownByIndex(const QModelIndex index);
	void tabPressed();
 private:
	void keyPressEvent(QKeyEvent *e) override;
	void mouseDoubleClickEvent(QMouseEvent *e) override;
	void doSelect ();
	void invertSelection();
 private:
	int m_current_pos;
	std::map<int, std::function<void()>> m_keys_map;
 };

#endif //VFS_SRC_EXTRA_UI_NAVIGATION_VIEW_HH_
