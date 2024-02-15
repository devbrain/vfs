//
// Created by igor on 2/15/24.
//

#include "mounts_dialog.hh"
#include "ui_mountsdialog.h"
#include <QStyle>


MountsDialog::MountsDialog (QWidget* parent, Qt::WindowFlags f)
	: QDialog (parent, f),
	 ui(new Ui::MountsDialog) {
	ui->setupUi (this);
}
