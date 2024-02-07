//
// Created by igor on 2/7/24.
//

#ifndef VFS_SRC_EXTRA_UI_NAVIGATION_VIEW_HH_
#define VFS_SRC_EXTRA_UI_NAVIGATION_VIEW_HH_

#include <QTreeView>

class NavigationView : public QTreeView {
	Q_OBJECT
 public:
	explicit NavigationView(QWidget* parent = nullptr);


 private:
	void keyPressEvent(QKeyEvent *e) override;
 signals:
	void onKeyUp();
	void onKeyDown();
};

#endif //VFS_SRC_EXTRA_UI_NAVIGATION_VIEW_HH_
