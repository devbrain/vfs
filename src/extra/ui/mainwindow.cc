//
// Created by igor on 2/4/24.
//

#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include "navigation_view.hh"

MainWindow::MainWindow (QWidget* parent)
	: QMainWindow (parent),
	  ui (new Ui::MainWindow) {
	ui->setupUi (this);
	ui->leftNavPanel->setActive (true);
	ui->rightNavPanel->setActive (false);

	setupSignalsForPanel (ui->leftNavPanel);
	setupSignalsForPanel (ui->rightNavPanel);
}

void MainWindow::setupSignalsForPanel(NavigationPanel* panel) {
	connect (panel, &NavigationPanel::tabPressed, this, &MainWindow::onTabPressed);
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
