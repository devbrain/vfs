//
// Created by igor on 2/15/24.
//

#include "mounts_dialog.hh"
#include "ui_mountsdialog.h"
#include "mounts_model.hh"

MountsDialog::MountsDialog (QWidget* parent, Qt::WindowFlags f)
	: QDialog (parent, f),
	 ui(new Ui::MountsDialog) {
	ui->setupUi (this);
	m_model = new MountsModel(this);
	ui->mountsView->setModel (m_model);
	for (int c = 0; c < ui->mountsView->horizontalHeader()->count(); ++c) {
		ui->mountsView->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);
	}
}
