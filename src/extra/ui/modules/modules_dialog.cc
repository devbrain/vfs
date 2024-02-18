//
// Created by igor on 2/16/24.
//

#include <QFileDialog>
#include <QMessageBox>

#include "modules_dialog.hh"
#include "modules_model.hh"
#include "ui_modules.h"

ModulesDialog::ModulesDialog (QWidget* parent, Qt::WindowFlags f)
: QDialog (parent, f),
  ui (new Ui::ModulesDialog) {
	ui->setupUi (this);
	m_model = new ModulesModel(this);
	ui->modulesView->setModel (m_model);

	for (int c = 0; c < ui->modulesView->horizontalHeader()->count(); ++c) {
		ui->modulesView->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);
	}

	connect (ui->fromFileButton, &QPushButton::clicked, this,  [this]() {loadModuleFromFile();});
	connect (ui->fromDirecoryButton, &QPushButton::clicked, this,  [this]() {loadModuleFromDirectory();});
}

void ModulesDialog::loadModuleFromFile () {
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
#if defined(Q_OS_WIN32)
	dialog.setNameFilter ("Module (*.dll)");
#else
	dialog.setNameFilter ("Module (*.so)");
#endif
	QStringList fileNames;
	if (dialog.exec()) {
		fileNames = dialog.selectedFiles ();
	}
	if (fileNames.size() == 1) {
		try {
			auto report = m_model->load (fileNames[0], true);
			if (!report.front().loaded) {
				QMessageBox::warning (this, "Failed to load module", QString("Failed to load ") + report.front().path);
			}
		} catch (std::exception& e) {
			QMessageBox::warning (this, "Failed to load module", e.what());
		}
	}
}

void ModulesDialog::loadModuleFromDirectory() {
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	QStringList fileNames;
	if (dialog.exec()) {
		fileNames = dialog.selectedFiles ();
	}
	if (fileNames.size() == 1) {
		try {
			m_model->load (fileNames[0], false);
		} catch (std::exception& e) {
			QMessageBox::warning (this, "Failed to load module", e.what());
		}
	}
}
