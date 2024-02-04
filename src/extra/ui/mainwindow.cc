//
// Created by igor on 2/4/24.
//

#include "mainwindow.hh"
#include "ui_mainwindow.h"

MainWindow::MainWindow (QWidget* parent)
	: QMainWindow (parent),
	  ui (new Ui::MainWindow) {
	ui->setupUi (this);
	ui->pushButton->setText ("Close Button");
}

MainWindow::~MainWindow () {
	delete ui;
}