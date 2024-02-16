//
// Created by igor on 2/4/24.
//

#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include "navigation/navigation_view.hh"
#include "mounts/mounts_dialog.hh"
#include "modules/modules_dialog.hh"

MainWindow::MainWindow (QWidget* parent)
	: QMainWindow (parent),
	  ui (new Ui::MainWindow) {
	ui->setupUi (this);
	mounts_dialog = new MountsDialog(this);
	modules_dialog = new ModulesDialog(this);

	ui->leftNavPanel->setActive (true);
	ui->rightNavPanel->setActive (false);

	setupSignalsForPanel (ui->leftNavPanel);
	setupSignalsForPanel (ui->rightNavPanel);

	connect (ui->actionQuit, &QAction::triggered, this, []() {
		qApp->quit();
	});

	connect (ui->actionMounts, &QAction::triggered, this, [this]() {
	  mounts_dialog->show();
	});

	connect (ui->actionModules, &QAction::triggered, this, [this]() {
		modules_dialog->show();
	});
}

void MainWindow::setupSignalsForPanel(NavigationPanel* panel) {
	connect (panel, &NavigationPanel::currentPathChanged, this, [this](const QString& newPath) {
		ui->currentPath->setText(newPath);
	});
}

MainWindow::~MainWindow () {
	delete ui;
}

void MainWindow::onTabPressed() {
	ui->leftNavPanel->toggleActive();
	ui->rightNavPanel->toggleActive();
}
