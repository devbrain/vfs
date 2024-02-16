//
// Created by igor on 2/16/24.
//

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
}
