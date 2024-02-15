//
// Created by igor on 2/15/24.
//

#ifndef VFS_SRC_EXTRA_UI_MOUNTS_DIALOG_HH_
#define VFS_SRC_EXTRA_UI_MOUNTS_DIALOG_HH_

#include <QDialog>

namespace Ui {
	class MountsDialog;
}

class MountsDialog : public QDialog {
 public:
	explicit MountsDialog (QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
 private:
	Ui::MountsDialog* ui;
};

#endif //VFS_SRC_EXTRA_UI_MOUNTS_DIALOG_HH_
