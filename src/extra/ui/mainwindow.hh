//
// Created by igor on 2/4/24.
//

#ifndef VFS_SRC_EXTRA_UI_MAINWINDOW_HH_
#define VFS_SRC_EXTRA_UI_MAINWINDOW_HH_

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class NavigationPanel;
class MountsDialog;
class ModulesDialog;

class MainWindow : public QMainWindow {
 Q_OBJECT
 public:
	explicit MainWindow (QWidget* parent = nullptr);
	~MainWindow () override;
 public slots:
	void onTabPressed();
 private:
	void setupSignalsForPanel(NavigationPanel* panel);
 private:
	Ui::MainWindow* ui;
	MountsDialog* mounts_dialog;
	ModulesDialog* modules_dialog;
};

#endif //VFS_SRC_EXTRA_UI_MAINWINDOW_HH_
